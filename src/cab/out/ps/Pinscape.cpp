

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

#include "PinscapeDevice.h"

namespace DOF
{

std::vector<PinscapeDevice*> Pinscape::m_devices = {};

void Pinscape::Initialize() { FindDevices(); }

Pinscape::Pinscape()
{
   m_pDevice = nullptr;
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
      std::ostringstream oss;
      oss << "Pinscape Controller " << std::setfill('0') << std::setw(2) << value;

      SetName(oss.str());
      m_number = value;

      for (PinscapeDevice* pDevice : m_devices)
      {
         if (pDevice->GetUnitNo() == value)
         {
            m_pDevice = pDevice;
            break;
         }
      }

      if (m_pDevice)
      {
         SetNumberOfOutputs(m_pDevice->GetNumOutputs());

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

void Pinscape::Init(Cabinet* pCabinet)
{

   if (!m_minCommandIntervalMsSet && pCabinet && pCabinet->GetOwner() && pCabinet->GetOwner()->HasConfigurationSetting("PinscapeDefaultMinCommandIntervalMs"))
   {
      std::string value = pCabinet->GetOwner()->GetConfigurationSetting("PinscapeDefaultMinCommandIntervalMs");
      try
      {
         int intervalMs = std::stoi(value);
         SetMinCommandIntervalMs(intervalMs);
      }
      catch (const std::exception&)
      {
      }
   }


   OutputControllerFlexCompleteBase::Init(pCabinet);
}

void Pinscape::Finish()
{
   if (m_pDevice)
      m_pDevice->AllOff();
   OutputControllerFlexCompleteBase::Finish();
}

void Pinscape::ConnectToController() { }

void Pinscape::DisconnectFromController()
{
   if (m_pDevice && m_inUseState == InUseState::Running)
      m_pDevice->AllOff();

   m_pDevice = nullptr;
   m_oldOutputValues.clear();
}

void Pinscape::AllOff()
{
   if (m_pDevice)
   {
      m_pDevice->AllOff();

      std::fill(m_oldOutputValues.begin(), m_oldOutputValues.end(), 0);
   }
}

int Pinscape::GetNumberOfConfiguredOutputs() { return m_pDevice ? m_pDevice->GetNumOutputs() : 32; }

int Pinscape::GetNumberOfOutputs() const { return m_pDevice ? m_pDevice->GetNumOutputs() : 32; }

void Pinscape::UpdateOutputs(const std::vector<uint8_t>& outputValues)
{
   if (!m_pDevice || outputValues.empty())
      return;

   uint8_t buf[9];
   uint8_t pfx = 200;

   for (int i = 0; i < GetNumberOfOutputs(); i += 7, ++pfx)
   {
      int lim = std::min(i + 7, GetNumberOfOutputs());
      for (int j = i; j < lim; ++j)
      {
         if (j < static_cast<int>(outputValues.size()) && outputValues[j] != m_oldOutputValues[j])
         {
            UpdateDelay();

            buf[0] = 0x00;
            buf[1] = pfx;

            int copySize = std::min(lim - i, static_cast<int>(outputValues.size()) - i);
            memcpy(buf + 2, outputValues.data() + i, copySize);

            m_pDevice->WriteUSB(buf);

            memcpy(m_oldOutputValues.data() + i, outputValues.data() + i, copySize);

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
         std::string productName = GetProductName(pCurrentDevice);
         std::string lowerProductName = StringExtensions::ToLower(productName);

         if (lowerProductName.find("pinscape") != std::string::npos)
         {
            if (pCurrentDevice->usage_page == 1 && (pCurrentDevice->usage == 4 || pCurrentDevice->usage == 0))
            {
               hid_device* pHandle = hid_open_path(pCurrentDevice->path);
               if (pHandle)
               {
                  PinscapeDevice* pDevice
                     = new PinscapeDevice(pHandle, pCurrentDevice->path, productName, pCurrentDevice->vendor_id, pCurrentDevice->product_id, pCurrentDevice->release_number);

                  Log::Write(StringExtensions::Build("Found Pinscape device: {0} (path={1})", pDevice->ToString(), pCurrentDevice->path));

                  m_devices.push_back(pDevice);
               }
            }
         }
      }

      pCurrentDevice = pCurrentDevice->next;
   }

   hid_free_enumeration(pDevices);
}

std::string Pinscape::GetProductName(hid_device_info* dev)
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

XMLElement* Pinscape::ToXml(XMLDocument& doc) const
{
   XMLElement* element = OutputControllerFlexCompleteBase::ToXml(doc);

   element->SetAttribute("Number", m_number);
   element->SetAttribute("MinCommandIntervalMs", m_minCommandIntervalMs);

   return element;
}

bool Pinscape::FromXml(const XMLElement* element)
{
   if (!OutputControllerFlexCompleteBase::FromXml(element))
      return false;

   int number = m_number;
   element->QueryIntAttribute("Number", &number);
   SetNumber(number);

   int minInterval = m_minCommandIntervalMs;
   element->QueryIntAttribute("MinCommandIntervalMs", &minInterval);
   SetMinCommandIntervalMs(minInterval);

   return true;
}

}
