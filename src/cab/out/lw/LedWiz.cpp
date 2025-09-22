#include "LedWiz.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include "../../Cabinet.h"
#include "../Output.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include <regex>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace DOF
{

std::vector<LedWiz::LWDEVICE> LedWiz::s_deviceList = {};

LedWiz::LedWiz()
{
   m_fp = nullptr;
   m_number = -1;
   m_minCommandIntervalMs = 10;
   m_minCommandIntervalMsSet = false;
}

LedWiz::LedWiz(int number)
   : LedWiz()
{
   SetNumber(number);
}

LedWiz::~LedWiz() { Finish(); }


void LedWiz::SetNumber(int value)
{
   if (value < 1 || value > 16)
   {
      Log::Write(StringExtensions::Build("LedWiz Numbers must be between 1-16. The supplied number {0} is out of range.", std::to_string(value)));
      return;
   }

   if (m_number != value)
   {
      if (GetName().empty() || GetName() == StringExtensions::Build("LedWiz {0:00}", std::to_string(m_number)))
      {
         SetName(StringExtensions::Build("LedWiz {0:00}", std::to_string(value)));
      }

      m_number = value;
   }
}

void LedWiz::SetMinCommandIntervalMs(int value)
{
   m_minCommandIntervalMs = std::clamp(value, 0, 1000);
   m_minCommandIntervalMsSet = true;
}

void LedWiz::Init(Cabinet* cabinet)
{
   Log::Write(StringExtensions::Build("Initializing LedWiz Nr. {0:00}", std::to_string(m_number)));
   AddOutputs();
   if (!m_minCommandIntervalMsSet && cabinet && cabinet->GetOwner() && cabinet->GetOwner()->HasConfigurationSetting("LedWizDefaultMinCommandIntervalMs"))
   {
      std::string value = cabinet->GetOwner()->GetConfigurationSetting("LedWizDefaultMinCommandIntervalMs");
      try
      {
         int intervalMs = std::stoi(value);
         SetMinCommandIntervalMs(intervalMs);
      }
      catch (const std::exception&)
      {
      }
   }

   ConnectToController();
   Log::Write(StringExtensions::Build("LedWiz Nr. {0:00} initialized and updater thread initialized.", std::to_string(m_number)));
}

void LedWiz::Finish()
{
   Log::Write(StringExtensions::Build("Finishing LedWiz Nr. {0:00}", std::to_string(m_number)));
   if (m_fp)
   {
      AllOff();
   }
   DisconnectFromController();
   OutputControllerCompleteBase::Finish();
   Log::Write(StringExtensions::Build("LedWiz Nr. {0:00} finished and updater thread stopped.", std::to_string(m_number)));
}

void LedWiz::Update() { }

void LedWiz::AddOutputs()
{
   OutputList* outputs = GetOutputs();
   if (!outputs)
      return;

   for (int i = 1; i <= 32; i++)
   {
      bool found = false;
      for (auto* output : *outputs)
      {
         if (output->GetNumber() == i)
         {
            found = true;
            break;
         }
      }

      if (!found)
      {
         Output* newOutput = new Output();
         newOutput->SetName(StringExtensions::Build("{0}.{1:00}", GetName(), std::to_string(i)));
         newOutput->SetNumber(i);
         outputs->push_back(newOutput);
      }
   }
}

bool LedWiz::IsConnected() const { return m_fp != nullptr; }

void LedWiz::AllOff()
{
   if (m_fp)
   {
      std::vector<uint8_t> buf = { 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      WriteUSB(buf);
   }
}

bool LedWiz::VerifySettings()
{
   if (m_number < 1 || m_number > 16)
   {
      Log::Write(StringExtensions::Build("LedWiz {0} cannot be configured. Valid LedWiz unit numbers are 1-16.", GetName()));
      return false;
   }
   return true;
}

void LedWiz::ConnectToController()
{
   if (!VerifySettings())
      return;

   for (const auto& device : s_deviceList)
   {
      if (device.unitNo == m_number)
      {
         m_fp = hid_open_path(device.path.c_str());
         if (m_fp)
         {
            m_path = device.path;
            Log::Write(StringExtensions::Build("LedWiz {0} connected successfully", GetName()));
            m_oldOutputValues.resize(32, 0);
            return;
         }
      }
   }

   Log::Write(StringExtensions::Build("LedWiz {0} could not connect to device", GetName()));
}

void LedWiz::DisconnectFromController()
{
   if (m_fp)
   {
      hid_close(m_fp);
      m_fp = nullptr;
   }
   m_path.clear();
   Log::Write(StringExtensions::Build("LedWiz {0} disconnected", GetName()));
}

void LedWiz::UpdateOutputs(const std::vector<uint8_t>& newOutputValues)
{
   if (!m_fp)
      return;

   if (newOutputValues.size() != m_oldOutputValues.size())
   {
      m_oldOutputValues.resize(newOutputValues.size(), 0);
   }

   bool hasChanges = false;
   for (size_t i = 0; i < std::min(newOutputValues.size(), size_t(32)); ++i)
   {
      if (newOutputValues[i] != m_oldOutputValues[i])
      {
         hasChanges = true;
         break;
      }
   }

   if (hasChanges)
   {
      std::vector<uint8_t> sbaCmd = { 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

      for (int i = 0; i < 32 && i < static_cast<int>(newOutputValues.size()); ++i)
      {
         int byteIndex = 2 + (i / 8);
         int bitIndex = i % 8;

         if (newOutputValues[i] > 127)
         {
            sbaCmd[byteIndex] |= (1 << bitIndex);
         }
      }

      WriteUSB(sbaCmd);

      for (int ofs = 0; ofs < 32; ofs += 8)
      {
         std::vector<uint8_t> pbaCmd = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

         for (int i = 0; i < 8; ++i)
         {
            int outputIndex = ofs + i;
            if (outputIndex < static_cast<int>(newOutputValues.size()))
            {
               pbaCmd[1 + i] = static_cast<uint8_t>((newOutputValues[outputIndex] * 49) / 255);
            }
         }

         WriteUSB(pbaCmd);
      }

      m_oldOutputValues = newOutputValues;
   }
}

bool LedWiz::WriteUSB(const std::vector<uint8_t>& data)
{
   if (!m_fp || data.empty())
      return false;

   int result = hid_write(m_fp, data.data(), data.size());
   if (result < 0)
   {
      Log::Write(StringExtensions::Build("LedWiz {0} WriteUSB failed after retries", std::to_string(m_number)));
      return false;
   }

   return true;
}

void LedWiz::FindDevices()
{
   s_deviceList.clear();

   hid_device_info* devs = hid_enumerate(0x0000, 0x0000);
   hid_device_info* curDev = devs;

   while (curDev)
   {
      std::string productName = GetDeviceProductName(curDev);
      std::string manufacturerName = GetDeviceManufacturerName(curDev);

      char vidStr[8], pidStr[8];
      snprintf(vidStr, sizeof(vidStr), "%04X", curDev->vendor_id);
      snprintf(pidStr, sizeof(pidStr), "%04X", curDev->product_id);

      Log::Instrumentation("LedWizDiscovery",
         StringExtensions::Build("Scanning HID at VID/PID: {0}/{1}, product string: {2}, manufacturer: {3}", std::string(vidStr), std::string(pidStr), productName, manufacturerName));

      bool ok = false;
      std::string okBecause;

      if (curDev->vendor_id == 0xFAFA)
      {
         ok = true;
         okBecause = "recognized by LedWiz vendor ID";
      }
      else if (curDev->vendor_id == 0x20A0)
      {
         std::regex zebsPattern("zebsboards", std::regex::ECMAScript | std::regex::icase);
         if (std::regex_search(manufacturerName, zebsPattern))
         {
            ok = true;
            okBecause = "recognized by ZebsBoards manufacturer";
         }
      }

      if (ok)
      {
         int unitNo = (curDev->product_id & 0x0F) + 1;
         if (unitNo < 1 || unitNo > 16)
            unitNo = 1;

         s_deviceList.emplace_back(unitNo, curDev->path, productName);

         Log::Write(StringExtensions::Build("Found LedWiz device: {0} (unit {1}, {2})", productName, std::to_string(unitNo), okBecause));
      }

      curDev = curDev->next;
   }

   hid_free_enumeration(devs);

   Log::Write(StringExtensions::Build("LedWiz device scan found {0} devices", std::to_string(s_deviceList.size())));
}

std::string LedWiz::GetDeviceProductName(hid_device_info* dev)
{
   if (dev->product_string)
   {
#ifdef _WIN32
      int size = WideCharToMultiByte(CP_UTF8, 0, dev->product_string, -1, nullptr, 0, nullptr, nullptr);
      if (size > 0)
      {
         std::string str(size - 1, 0);
         WideCharToMultiByte(CP_UTF8, 0, dev->product_string, -1, &str[0], size, nullptr, nullptr);
         return str;
      }
      return "<not available>";
#else
      std::wstring wstr(dev->product_string);
      std::string str(wstr.begin(), wstr.end());
      return str;
#endif
   }
   return "<not available>";
}

std::string LedWiz::GetDeviceManufacturerName(hid_device_info* dev)
{
   if (dev->manufacturer_string)
   {
#ifdef _WIN32
      int size = WideCharToMultiByte(CP_UTF8, 0, dev->manufacturer_string, -1, nullptr, 0, nullptr, nullptr);
      if (size > 0)
      {
         std::string str(size - 1, 0);
         WideCharToMultiByte(CP_UTF8, 0, dev->manufacturer_string, -1, &str[0], size, nullptr, nullptr);
         return str;
      }
      return "<not available>";
#else
      std::wstring wstr(dev->manufacturer_string);
      std::string str(wstr.begin(), wstr.end());
      return str;
#endif
   }
   return "<not available>";
}

std::vector<int> LedWiz::GetLedwizNumbers()
{
   std::vector<int> numbers;
   for (const auto& device : s_deviceList)
   {
      numbers.push_back(device.unitNo);
   }
   return numbers;
}

tinyxml2::XMLElement* LedWiz::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = OutputControllerCompleteBase::ToXml(doc);

   tinyxml2::XMLElement* numberElement = doc.NewElement("Number");
   numberElement->SetText(m_number);
   element->InsertEndChild(numberElement);

   tinyxml2::XMLElement* intervalElement = doc.NewElement("MinCommandIntervalMs");
   intervalElement->SetText(m_minCommandIntervalMs);
   element->InsertEndChild(intervalElement);

   return element;
}

bool LedWiz::FromXml(const tinyxml2::XMLElement* element)
{
   if (!OutputControllerCompleteBase::FromXml(element))
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

}