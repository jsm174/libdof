/*
 * Portions of this code was derived from Direct Output Framework
 *
 * https://github.com/mjrgh/DirectOutput/blob/master/DirectOutput/Cab/Out/PS/Pinscape.cs
 */

#include "Pinscape.h"

#include <algorithm>
#include <cstdint>

#include "Logger.h"
#include "PinscapeDevice.h"

namespace DOF
{

Pinscape::Pinscape() {}

Pinscape::~Pinscape()
{
  for (PinscapeDevice* pDevice : m_devices)
  {
    delete pDevice;
  }
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

void Pinscape::DataReceive(char type, int number, int value)
{
  for (PinscapeDevice* pDevice : m_devices)
  {
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
