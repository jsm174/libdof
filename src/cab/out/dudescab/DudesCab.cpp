#include "DudesCab.h"
#include "../../Cabinet.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include "../../../general/MathExtensions.h"
#include <algorithm>
#include <cstring>
#include <thread>

#ifdef __HIDAPI__
#include <hidapi/hidapi.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace DOF
{

std::vector<DudesCab::Device*> DudesCab::s_devices;

DudesCab::DudesCab()
   : m_number(-1)
   , m_minCommandIntervalMs(1)
   , m_minCommandIntervalMsSet(false)
   , m_dev(nullptr)
   , m_lastUpdate(std::chrono::steady_clock::now())
{
}

DudesCab::DudesCab(int number)
   : DudesCab()
{
   SetNumber(number);
}

DudesCab::~DudesCab() { Finish(); }

void DudesCab::SetNumber(int value)
{
   if (!MathExtensions::IsBetween(value, 1, 5))
      throw std::runtime_error(StringExtensions::Build("DudesCab Unit Numbers must be between 1-5. The supplied number {0} is out of range.", std::to_string(value)));

   std::lock_guard<std::mutex> lock(m_numberUpdateLocker);

   if (m_number != value)
   {
      if (GetName().empty() || GetName() == StringExtensions::Build("DudesCab Controller {0:00}", std::to_string(m_number)))
         SetName(StringExtensions::Build("DudesCab Controller {0:00}", std::to_string(value)));

      m_number = value;

      auto it = std::find_if(s_devices.begin(), s_devices.end(), [this](Device* d) { return d->UnitNo() == m_number; });

      if (it != s_devices.end())
      {
         m_dev = *it;
         SetNumberOfOutputs(m_dev->NumOutputs());
         m_oldOutputValues.assign(GetNumberOfOutputs(), 255);
      }
   }
}

void DudesCab::SetMinCommandIntervalMs(int value)
{
   m_minCommandIntervalMs = MathExtensions::Limit(value, 0, 1000);
   m_minCommandIntervalMsSet = true;
}

void DudesCab::Init(Cabinet* cabinet)
{
   if (!m_minCommandIntervalMsSet && cabinet && cabinet->GetOwner()
      && cabinet->GetOwner()->GetConfigurationSettings().find("DudesCabDefaultMinCommandIntervalMs") != cabinet->GetOwner()->GetConfigurationSettings().end())
      m_minCommandIntervalMs = 1;

   OutputControllerFlexCompleteBase::Init(cabinet);
}

void DudesCab::Finish()
{
   if (m_dev)
      m_dev->AllOff();
   OutputControllerFlexCompleteBase::Finish();
}

bool DudesCab::VerifySettings() { return true; }

void DudesCab::Instrumentation(const std::string& message) { Log::Instrumentation("DudesCab", message); }

void DudesCab::UpdateOutputs(const std::vector<uint8_t>& newOutputValues)
{
   if (std::all_of(newOutputValues.begin(), newOutputValues.end(), [](uint8_t x) { return x == 0; }))
   {
      if (m_dev)
         m_dev->AllOff();
   }
   else
   {
      m_outputBuffer.clear();
      m_outputBuffer.push_back(0);
      int nbValuesToSend = 0;

      uint8_t extMask = 0;
      uint8_t oldExtMask = 0xFF;
      int outputMaskOffset = 0;
      uint16_t outputMask = 0;

      for (int numDofOutput = 0; numDofOutput < static_cast<int>(newOutputValues.size()); numDofOutput++)
      {
         if (m_oldOutputValues.size() <= static_cast<size_t>(numDofOutput) || newOutputValues[numDofOutput] != m_oldOutputValues[numDofOutput])
         {
            uint8_t extNum = static_cast<uint8_t>(numDofOutput / m_dev->PwmMaxOutputsPerExtension);
            uint8_t outputNum = static_cast<uint8_t>(numDofOutput % m_dev->PwmMaxOutputsPerExtension);

            std::string oldVal = m_oldOutputValues.size() > static_cast<size_t>(numDofOutput) ? std::to_string(m_oldOutputValues[numDofOutput]) : "0";
            Instrumentation(StringExtensions::Build("Prepare Dof Value to send : DOF #{0} {1} => {2}, Extension #{3}, Output #{4}",
               { std::to_string(numDofOutput), oldVal, std::to_string(newOutputValues[numDofOutput]), std::to_string(extNum), std::to_string(outputNum) }));

            extMask |= (1 << extNum);
            if (oldExtMask != extMask)
            {
               oldExtMask = extMask;
               if (outputMask != 0)
               {
                  m_outputBuffer[outputMaskOffset] = static_cast<uint8_t>(outputMask & 0xFF);
                  m_outputBuffer[outputMaskOffset + 1] = static_cast<uint8_t>((outputMask >> 8) & 0xFF);
                  Instrumentation(StringExtensions::Build("        Changed OutputMask 0x{0:X4}", std::to_string(outputMask)));
                  outputMask = 0;
               }
               Instrumentation(StringExtensions::Build("    Extension {0} has changes", std::to_string(extNum)));
               outputMaskOffset = static_cast<int>(m_outputBuffer.size());
               m_outputBuffer.push_back(0);
               m_outputBuffer.push_back(0);
            }

            outputMask |= (1 << outputNum);
            m_outputBuffer.push_back(newOutputValues[numDofOutput]);
            nbValuesToSend++;
         }
      }

      if (outputMask != 0)
      {
         m_outputBuffer[outputMaskOffset] = static_cast<uint8_t>(outputMask & 0xFF);
         m_outputBuffer[outputMaskOffset + 1] = static_cast<uint8_t>((outputMask >> 8) & 0xFF);
         Instrumentation(StringExtensions::Build("        Changed OutputMask 0x{0:X4}", std::to_string(outputMask)));
      }
      m_outputBuffer[0] = extMask;
      Instrumentation(StringExtensions::Build("    ExtenstionMask 0x{0:X2}", std::to_string(m_outputBuffer[0])));

      Instrumentation(StringExtensions::Build("{0} Dof Values to send to Dude's cab", std::to_string(nbValuesToSend)));

      if (m_dev)
         m_dev->SendCommand(Device::RT_PWM_OUTPUTS, m_outputBuffer);
   }

   m_oldOutputValues = newOutputValues;
}

void DudesCab::UpdateDelay()
{
   auto now = std::chrono::steady_clock::now();
   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count();

   if (elapsed < m_minCommandIntervalMs)
      std::this_thread::sleep_for(std::chrono::milliseconds(m_minCommandIntervalMs - elapsed));
   m_lastUpdate = std::chrono::steady_clock::now();
}

void DudesCab::ConnectToController() { }

void DudesCab::DisconnectFromController()
{
   if (m_inUseState == InUseState::Running && m_dev)
      m_dev->AllOff();
}

std::vector<DudesCab::Device*> DudesCab::AllDevices()
{
   if (s_devices.empty())
      s_devices = FindDevices();
   return s_devices;
}

std::vector<DudesCab::Device*> DudesCab::FindDevices()
{
   std::vector<Device*> devices;

#ifdef __HIDAPI__
   struct hid_device_info* devs = hid_enumerate(VendorID, ProductID);
   struct hid_device_info* cur_dev = devs;

   while (cur_dev)
   {
      std::string productName;
      if (cur_dev->product_string)
      {
#ifdef _WIN32
         int size = WideCharToMultiByte(CP_UTF8, 0, cur_dev->product_string, -1, nullptr, 0, nullptr, nullptr);
         if (size > 0)
         {
            productName.resize(size - 1);
            WideCharToMultiByte(CP_UTF8, 0, cur_dev->product_string, -1, &productName[0], size, nullptr, nullptr);
         }
         else
         {
            productName = "<not available>";
         }
#else
         productName = std::string(reinterpret_cast<const char*>(cur_dev->product_string));
#endif
      }

      if (!productName.empty() && productName.find("DudesCab Outputs") != std::string::npos)
      {
         std::string serialNumber;
         if (cur_dev->serial_number)
         {
#ifdef _WIN32
            int size = WideCharToMultiByte(CP_UTF8, 0, cur_dev->serial_number, -1, nullptr, 0, nullptr, nullptr);
            if (size > 0)
            {
               serialNumber.resize(size - 1);
               WideCharToMultiByte(CP_UTF8, 0, cur_dev->serial_number, -1, &serialNumber[0], size, nullptr, nullptr);
            }
            else
            {
               serialNumber = "<not available>";
            }
#else
            serialNumber = std::string(reinterpret_cast<const char*>(cur_dev->serial_number));
#endif
         }
         else
            serialNumber = "<not available>";

         bool alreadyEnumerated = false;
         for (Device* existingDevice : devices)
         {
            if (existingDevice->GetSerial() == serialNumber)
            {
               alreadyEnumerated = true;
               break;
            }
         }

         if (!alreadyEnumerated)
         {
            Device* device = new Device(cur_dev->path, productName.empty() ? "<not available>" : productName, serialNumber, cur_dev->vendor_id, cur_dev->product_id, cur_dev->release_number);
            devices.push_back(device);
         }
      }
      cur_dev = cur_dev->next;
   }

   hid_free_enumeration(devs);
#endif

   return devices;
}

DudesCab::Device::Device(const std::string& path, const std::string& name, const std::string& serial, uint16_t vendorID, uint16_t productID, uint16_t version)
   : m_path(path)
   , m_name(name)
   , m_serial(serial)
   , m_vendorID(vendorID)
   , m_productID(productID)
   , m_version(version)
   , m_unitNo(0)
   , m_numOutputs(128)
   , m_maxExtensions(0)
   , PwmMaxOutputsPerExtension(0)
   , PwmExtensionsMask(0)
   , m_hidDevice(nullptr)
{
#ifdef __HIDAPI__
   m_hidDevice = hid_open_path(path.c_str());
   if (m_hidDevice)
   {
      SendCommand(RT_HANDSHAKE);
      std::vector<uint8_t> answer = ReadUSB();
      if (answer.size() > HID_COMMAND_PREFIX_SIZE)
      {
         std::string handShake(answer.begin() + HID_COMMAND_PREFIX_SIZE, answer.end());
         handShake.erase(std::find(handShake.begin(), handShake.end(), '\0'), handShake.end());

         size_t pipePos = handShake.find('|');
         if (pipePos != std::string::npos)
         {
            m_name = handShake.substr(0, pipePos);
            handShake = handShake.substr(pipePos + 1);
         }
         else
         {
            Log::Warning("Old Dude's Cab handshake, you should update your firmware");
         }
         Log::Write(StringExtensions::Build("{0} says : {1}", m_name, handShake));
      }

      SendCommand(RT_INFOS);
      answer = ReadUSB();
      if (answer.size() > HID_COMMAND_PREFIX_SIZE)
      {
         std::vector<uint8_t> info(answer.begin() + HID_COMMAND_PREFIX_SIZE, answer.end());
         if (info.size() >= 5)
         {
            Log::Write(StringExtensions::Build("DudesCab Controller Informations : Name [{0}], v{1}.{2}.{3}, unit #{4}, Max extensions {5}",
               { m_name, std::to_string(info[0]), std::to_string(info[1]), std::to_string(info[2]), std::to_string(info[3]), std::to_string(info[4]) }));
            m_unitNo = info[3];
            m_maxExtensions = info[4];
         }
      }

      SendCommand(RT_PWM_GETEXTENSIONSINFOS);
      answer = ReadUSB();
      if (answer.size() > HID_COMMAND_PREFIX_SIZE)
      {
         std::vector<uint8_t> pwmInfo(answer.begin() + HID_COMMAND_PREFIX_SIZE, answer.end());
         if (pwmInfo.size() >= 2)
         {
            PwmMaxOutputsPerExtension = pwmInfo[0];
            PwmExtensionsMask = pwmInfo[1];
            Log::Write(StringExtensions::Build(
               "    Pwm Informations : Max outputs per extensions {0}, Extension Mask 0x{1}", { std::to_string(PwmMaxOutputsPerExtension), std::to_string(PwmExtensionsMask) }));

            if (pwmInfo.size() > 2)
            {
               m_numOutputs = 0;
               uint8_t nbMasks = pwmInfo[2];
               if (pwmInfo.size() >= 4 + (nbMasks * 2))
               {
                  for (int ext = 0; ext < m_maxExtensions; ext++)
                  {
                     if ((PwmExtensionsMask & (1 << ext)) != 0)
                     {
                        m_numOutputs = std::max(m_numOutputs, (ext + 1) * PwmMaxOutputsPerExtension);
                     }
                  }
               }
               Log::Write(StringExtensions::Build("    Output configuration received, highest configured output is #{0}", std::to_string(m_numOutputs)));
            }
         }
      }
   }
#endif
}

DudesCab::Device::~Device()
{
#ifdef __HIDAPI__
   if (m_hidDevice)
   {
      hid_close(m_hidDevice);
   }
#endif
}

std::string DudesCab::Device::ToString() const { return StringExtensions::Build("{0} (unit {1})", m_name, std::to_string(m_unitNo)); }

std::vector<uint8_t> DudesCab::Device::ReadUSB()
{
   std::vector<uint8_t> result;

#ifdef __HIDAPI__
   if (m_hidDevice)
   {
      uint8_t buffer[65];
      int bytes_read = hid_read_timeout(m_hidDevice, buffer, sizeof(buffer), 1000);
      if (bytes_read > 0)
      {
         result.assign(buffer, buffer + bytes_read);
      }
   }
#endif

   return result;
}

bool DudesCab::Device::WriteUSB(const std::vector<uint8_t>& data)
{
#ifdef __HIDAPI__
   if (m_hidDevice && !data.empty())
   {
      int result = hid_write(m_hidDevice, data.data(), data.size());
      return result >= 0;
   }
#endif
   return false;
}

void DudesCab::Device::AllOff() { SendCommand(RT_PWM_ALLOFF); }

void DudesCab::Device::SendCommand(HIDReportType command, const std::vector<uint8_t>& parameters)
{
#ifdef __HIDAPI__
   if (!m_hidDevice)
      return;

   Log::Instrumentation("DudesCab", StringExtensions::Build("DudesCab SendCommand: {0}", std::to_string(static_cast<int>(command))));

   uint8_t bufferOffset = 5;
   uint8_t partSize = 60;
   uint8_t nbParts = static_cast<uint8_t>((parameters.size() / partSize) + 1);

   for (uint8_t i = 0; i < nbParts; i++)
   {
      std::vector<uint8_t> sendData(bufferOffset);
      sendData[0] = RID_OUTPUTS;
      sendData[1] = static_cast<uint8_t>(command);
      sendData[2] = i;
      sendData[3] = nbParts;

      size_t remainingData = parameters.size() - (i * partSize);
      uint8_t toSend = static_cast<uint8_t>(std::min(static_cast<size_t>(partSize), remainingData));
      sendData[4] = toSend;

      if (toSend > 0)
      {
         size_t startPos = i * partSize;
         sendData.insert(sendData.end(), parameters.begin() + startPos, parameters.begin() + startPos + toSend);
      }

      WriteUSB(sendData);
   }
#endif
}

bool DudesCab::FromXml(const tinyxml2::XMLElement* element)
{
   if (!OutputControllerFlexCompleteBase::FromXml(element))
      return false;

   const tinyxml2::XMLElement* numberElement = element->FirstChildElement("Number");
   if (numberElement && numberElement->GetText())
   {
      try
      {
         int number = std::stoi(numberElement->GetText());
         SetNumber(number);
      }
      catch (...)
      {
         return false;
      }
   }

   const tinyxml2::XMLElement* intervalElement = element->FirstChildElement("MinCommandIntervalMs");
   if (intervalElement && intervalElement->GetText())
   {
      try
      {
         int interval = std::stoi(intervalElement->GetText());
         SetMinCommandIntervalMs(interval);
      }
      catch (...)
      {
      }
   }

   return true;
}

tinyxml2::XMLElement* DudesCab::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = OutputControllerFlexCompleteBase::ToXml(doc);

   tinyxml2::XMLElement* numberElement = doc.NewElement("Number");
   numberElement->SetText(m_number);
   element->InsertEndChild(numberElement);

   tinyxml2::XMLElement* intervalElement = doc.NewElement("MinCommandIntervalMs");
   intervalElement->SetText(m_minCommandIntervalMs);
   element->InsertEndChild(intervalElement);

   return element;
}

}