#include "Pinscape.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include "../../Cabinet.h"

namespace DOF
{

std::vector<Pinscape::Device*> Pinscape::s_devices = {};

void Pinscape::Initialize() { FindDevices(); }

Pinscape::Pinscape()
{
   m_dev = nullptr;
   m_number = -1;
   m_minCommandIntervalMs = 1;
   m_minCommandIntervalMsSet = false;
   m_lastUpdate = std::chrono::steady_clock::now();
}

Pinscape::Pinscape(int value)
   : Pinscape()
{
   SetNumber(value);
}

Pinscape::~Pinscape() { }

void Pinscape::SetNumber(int value)
{
   if (value < 1 || value > 16)
   {
      Log::Write(StringExtensions::Build("Pinscape Unit Numbers must be between 1-16. The supplied number {0} is out of range.", std::to_string(value)));
      return;
   }

   if (m_number != value)
   {
      if (GetName().empty() || GetName() == StringExtensions::Build("Pinscape Controller {0:00}", std::to_string(m_number)))
      {
         SetName(StringExtensions::Build("Pinscape Controller {0:00}", std::to_string(value)));
      }

      m_number = value;

      for (Device* device : s_devices)
      {
         if (device->GetUnitNo() == value)
         {
            m_dev = device;
            break;
         }
      }

      if (m_dev)
      {
         SetNumberOfOutputs(m_dev->GetNumOutputs());
         m_oldOutputValues.resize(GetNumberOfOutputs(), 255);
      }
   }
}

bool Pinscape::VerifySettings() { return true; }

void Pinscape::SetMinCommandIntervalMs(int value)
{
   m_minCommandIntervalMs = std::clamp(value, 0, 1000);
   m_minCommandIntervalMsSet = true;
}

void Pinscape::Init(Cabinet* cabinet)
{
   if (!m_minCommandIntervalMsSet && cabinet && cabinet->GetOwner() && cabinet->GetOwner()->HasConfigurationSetting("PinscapeDefaultMinCommandIntervalMs"))
   {
      std::string value = cabinet->GetOwner()->GetConfigurationSetting("PinscapeDefaultMinCommandIntervalMs");
      try
      {
         int intervalMs = std::stoi(value);
         SetMinCommandIntervalMs(intervalMs);
      }
      catch (const std::exception&)
      {
      }
   }

   OutputControllerFlexCompleteBase::Init(cabinet);
}

void Pinscape::Finish()
{
   if (m_dev)
      m_dev->AllOff();
   OutputControllerFlexCompleteBase::Finish();
}

void Pinscape::ConnectToController() { }

void Pinscape::DisconnectFromController()
{
   if (m_dev && m_inUseState == InUseState::Running)
      m_dev->AllOff();
}

void Pinscape::AllOff()
{
   if (m_dev)
   {
      m_dev->AllOff();
      std::fill(m_oldOutputValues.begin(), m_oldOutputValues.end(), 0);
   }
}

int Pinscape::GetNumberOfConfiguredOutputs() { return GetNumberOfOutputs(); }

int Pinscape::GetNumberOfOutputs() const { return m_dev ? m_dev->GetNumOutputs() : 32; }

void Pinscape::UpdateOutputs(const std::vector<uint8_t>& newOutputValues)
{
   if (!m_dev)
      return;

   uint8_t pfx = 200;
   for (int i = 0; i < GetNumberOfOutputs(); i += 7, ++pfx)
   {
      int lim = std::min(i + 7, GetNumberOfOutputs());
      for (int j = i; j < lim; ++j)
      {
         if (j < static_cast<int>(newOutputValues.size()) && newOutputValues[j] != m_oldOutputValues[j])
         {
            UpdateDelay();

            uint8_t buf[9] = { 0 };
            buf[0] = 0;
            buf[1] = pfx;

            int copySize = std::min(lim - i, static_cast<int>(newOutputValues.size()) - i);
            memcpy(buf + 2, newOutputValues.data() + i, copySize);

            m_dev->WriteUSB(buf);

            memcpy(m_oldOutputValues.data() + i, newOutputValues.data() + i, copySize);

            break;
         }
      }
   }
}

void Pinscape::UpdateDelay()
{
   auto now = std::chrono::steady_clock::now();
   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count();

   if (elapsed < m_minCommandIntervalMs)
      std::this_thread::sleep_for(std::chrono::milliseconds(m_minCommandIntervalMs - elapsed));

   m_lastUpdate = now;
}

void Pinscape::FindDevices()
{
   hid_device_info* pDevices = hid_enumerate(0x0, 0x0);
   hid_device_info* pCurrentDevice = pDevices;

   while (pCurrentDevice)
   {
      bool isLedWiz = pCurrentDevice->vendor_id == 0xFAFA && (pCurrentDevice->product_id >= 0x00F0 && pCurrentDevice->product_id <= 0x00FF);
      bool isPinscape = pCurrentDevice->vendor_id == 0x1209 && pCurrentDevice->product_id == 0xEAEA;

      if ((isLedWiz || isPinscape) && pCurrentDevice->release_number > 7)
      {
         std::string productName = GetDeviceProductName(pCurrentDevice);
         std::string lowerProductName = StringExtensions::ToLower(productName);

         if (lowerProductName.find("pinscape") != std::string::npos)
         {
            if (pCurrentDevice->usage_page == 1 && (pCurrentDevice->usage == 4 || pCurrentDevice->usage == 0))
            {
               hid_device* pHandle = hid_open_path(pCurrentDevice->path);
               if (pHandle)
               {
                  Device* pDevice = new Device(pHandle, pCurrentDevice->path, productName, pCurrentDevice->vendor_id, pCurrentDevice->product_id, pCurrentDevice->release_number);

                  Log::Write(StringExtensions::Build("Found Pinscape device: {0} (path={1})", pDevice->ToString(), pCurrentDevice->path));

                  s_devices.push_back(pDevice);
               }
            }
         }
      }

      pCurrentDevice = pCurrentDevice->next;
   }

   hid_free_enumeration(pDevices);
}

std::string Pinscape::GetDeviceProductName(hid_device_info* dev)
{
   std::string productName;
   if (dev->product_string)
   {
      wchar_t* wstr = dev->product_string;
      while (*wstr)
         productName += static_cast<char>(*wstr++);
   }
   return productName;
}

tinyxml2::XMLElement* Pinscape::ToXml(tinyxml2::XMLDocument& doc) const
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

bool Pinscape::FromXml(const tinyxml2::XMLElement* element)
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

std::vector<void*> Pinscape::GetAllDevices()
{
   std::vector<void*> devicePointers;
   for (Device* pDevice : s_devices)
   {
      devicePointers.push_back(static_cast<void*>(pDevice));
   }
   return devicePointers;
}

Pinscape::Device::Device(hid_device* pDevice, const std::string& path, const std::string& name, uint16_t vendorID, uint16_t productID, uint16_t version)
{
   m_pDevice = pDevice;
   m_path = path;
   m_name = name;
   m_vendorID = vendorID;
   m_productID = productID;
   m_version = version;
   m_plungerEnabled = true;
   m_numOutputs = 32;

   if (vendorID == 0xFAFA && (productID & 0xFFF0) == 0x00F0)
      m_unitNo = (int)((productID & 0x000f) + 1);
   else
      m_unitNo = 1;

   uint8_t buf[15];
   if (ReadUSB(buf))
   {
      m_plungerEnabled = (buf[1] & 0x01) != 0;
   }

   SpecialRequest(0x04);

   for (int i = 0; i < 16; ++i)
   {
      if (ReadUSB(buf))
      {
         if ((buf[2] & 0xF8) == 0x88)
         {
            m_numOutputs = (int)buf[3] | (((int)buf[4]) << 8);
            m_unitNo = (int)(((uint16_t)buf[5] | (((uint16_t)buf[6]) << 8)) + 1);

            break;
         }
      }
   }
}

Pinscape::Device::~Device() { hid_close(m_pDevice); }

bool Pinscape::Device::IsLedWizEmulator(int unitNum) { return (uint16_t)m_vendorID == 0xFAFA && m_productID == 0x00F1 + unitNum; }

bool Pinscape::Device::ReadUSB(uint8_t* pBuf)
{
   memset(pBuf, 0x00, 15);

   int actual = hid_read(m_pDevice, pBuf + 1, 14);
   if (actual != 14)
   {
      Log::Write("Pinscape Controller USB error reading from device: not all bytes received");
      return false;
   }

   return true;
}

void Pinscape::Device::AllOff() { SpecialRequest(0x05); }

bool Pinscape::Device::SpecialRequest(uint8_t id)
{
   uint8_t buf[9];
   memset(buf, 0x00, sizeof(buf));

   buf[1] = 0x41;
   buf[2] = id;

   return WriteUSB(buf);
}

bool Pinscape::Device::WriteUSB(uint8_t* pBuf)
{
   int actual = hid_write(m_pDevice, pBuf, 9);
   if (actual != 9)
   {
      Log::Write("Pinscape Controller USB error sending request: not all bytes sent");
      return false;
   }

   return true;
}

void Pinscape::Device::UpdateOutputs(uint8_t* NewOutputValues)
{
   // Device-specific update implementation would go here
   // For now, this can be empty as UpdateOutputs is handled at the controller level
}

}
