/*
 * Portions of this code was derived from Direct Output Framework
 *
 * https://github.com/mjrgh/DirectOutput/blob/master/DirectOutput/Cab/Out/PS/Pinscape.cs
 */

#include "Pinscape.h"

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

#include "Logger.h"
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

Pinscape::Pinscape(int value) : Pinscape() { SetNumber(value); }

Pinscape::~Pinscape() {}

void Pinscape::SetNumber(int value)
{
  if (value < 1 || value > 16)
  {
    Log("Pinscape Unit Numbers must be between 1-16. The supplied number %d is out of range.", value);
    return;
  }
  if (m_number != value)
  {
    std::ostringstream oss;
    oss << "Pinscape Controller " << std::setfill('0') << std::setw(2) << value;

    m_szName = oss.str();
    m_number = value;

    for (PinscapeDevice* pDevice : m_devices)
    {
      if (pDevice->GetUnitNo() == value)
      {
        m_pDevice = pDevice;
        break;
      }
    }

    if (m_pDevice) m_numberOfOutputs = m_pDevice->GetNumOutputs();

    memset(m_oldOutputValues, 255, m_numberOfOutputs);
  }
}

void Pinscape::SetMinCommandIntervalMs(int value)
{
  m_minCommandIntervalMs = std::clamp(value, 0, 1000);
  m_minCommandIntervalMsSet = true;
}

void Pinscape::Init()
{
  if (!m_minCommandIntervalMsSet 
       /*&& Cabinet.Owner.ConfigurationSettings.ContainsKey("PinscapeDefaultMinCommandIntervalMs")
         && Cabinet.Owner.ConfigurationSettings["PinscapeDefaultMinCommandIntervalMs"] is int*/)
  {
    SetMinCommandIntervalMs(1);  //(int)Cabinet.Owner.ConfigurationSettings["PinscapeDefaultMinCommandIntervalMs"];*/
  }

  /*base.Init(Cabinet);*/
}

void Pinscape::Finish()
{
  m_pDevice->AllOff();

  /*base.Finish();*/
}

void Pinscape::UpdateOutputs(uint8_t* pNewOutputValues)
{
  uint8_t pfx = 200;
  for (int i = 0; i < m_numberOfOutputs; i += 7, ++pfx)
  {
    int lim = std::min(i + 7, m_numberOfOutputs);
    for (int j = i; j < lim; ++j)
    {
      if (pNewOutputValues[j] != m_oldOutputValues[j])
      {
        UpdateDelay();
        uint8_t buf[9] = {0};
        buf[0] = 0;
        buf[1] = pfx;

        // Array.Copy(NewOutputValues, i, buf, 2, lim - i);
        memcpy(buf + 2, pNewOutputValues + i, lim - i);

        m_pDevice->WriteUSB(buf);

        // Array.Copy(NewOutputValues, i, OldOutputValues, i, lim - i);
        memcpy(m_oldOutputValues + i, pNewOutputValues + i, lim - i);

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
    bool isLedWiz = pCurrentDevice->vendor_id == 0xFAFA &&
                    (pCurrentDevice->product_id >= 0x00F0 && pCurrentDevice->product_id <= 0x00FF);
    bool isPinscape = pCurrentDevice->vendor_id == 0x1209 && pCurrentDevice->product_id == 0xEAEA;

    if ((isLedWiz || isPinscape) && pCurrentDevice->release_number > 7)
    {
      std::string productName = GetProductName(pCurrentDevice);
      std::string lowerProductName = productName;

      std::transform(lowerProductName.begin(), lowerProductName.end(), lowerProductName.begin(),
                     [](unsigned char c) -> unsigned char { return std::tolower(c); });

      if (lowerProductName.find("pinscape") != std::string::npos)
      {
        if (pCurrentDevice->usage_page == 1 && (pCurrentDevice->usage == 4 || pCurrentDevice->usage == 0))
        {
          hid_device* pHandle = hid_open_path(pCurrentDevice->path);
          if (pHandle)
          {
            PinscapeDevice* pDevice =
                new PinscapeDevice(pHandle, pCurrentDevice->path, productName, pCurrentDevice->vendor_id,
                                   pCurrentDevice->product_id, pCurrentDevice->release_number);

            Log("Found Pinscape device: %s", pDevice->ToString().c_str());

            m_devices.push_back(pDevice);
          }
        }
      }
    }

    pCurrentDevice = pCurrentDevice->next;
  }
}

std::string Pinscape::GetProductName(hid_device_info* dev)
{
  std::string productName;
  if (dev->product_string)
  {
    wchar_t* wstr = dev->product_string;
    while (*wstr) productName += static_cast<char>(*wstr++);
  }
  return productName;
}

}  // namespace DOF
