/*
 * Portions of this code was derived from Direct Output Framework
 *
 * https://github.com/mjrgh/DirectOutput/blob/master/DirectOutput/Cab/Out/PS/Pinscape.cs
 */

#include "PinscapeDevice.h"

#include <algorithm>
#include <cstdint>

#include "Logger.h"

namespace DOF
{

PinscapeDevice::PinscapeDevice(hid_device* pDevice, const std::string& szPath, const std::string& szName,
                               unsigned short vendorID, unsigned short productID, unsigned short version)
{
  m_pDevice = pDevice;
  m_szPath = szPath;
  m_szName = szName;
  m_vendorID = vendorID;
  m_productID = productID;
  m_version = version;
  m_plungerEnabled = true;
  m_numOutputs = 32;

  if (vendorID == 0xFAFA && (productID & 0xFFF0) == 0x00F0)
    m_unitNo = (unsigned short)((productID & 0x000f) + 1);
  else
    m_unitNo = 1;

  uint8_t buffer[15] = {0};
  if (ReadUSB(buffer))
  {
    m_plungerEnabled = (buffer[1] & 0x01) != 0;
  }

  SpecialRequest(4);

  for (int i = 0; i < 16; ++i)
  {
    if (ReadUSB(buffer))
    {
      if ((buffer[2] & 0xF8) == 0x88)
      {
        m_numOutputs = (int)buffer[3] | (((int)buffer[4]) << 8);
        m_unitNo = (short)(((ushort)buffer[5] | (((ushort)buffer[6]) << 8)) + 1);

        break;
      }
    }
  }
}

PinscapeDevice::~PinscapeDevice() { hid_close(m_pDevice); }

bool PinscapeDevice::IsLedWizEmulator(int unitNum)
{
  return (ushort)m_vendorID == 0xFAFA && m_productID == 0x00F1 + unitNum;
}

bool PinscapeDevice::ReadUSB(uint8_t* pBuffer)
{
  pBuffer[0] = 0x00;

  int actual = hid_read(m_pDevice, pBuffer, 15);

  if (actual != 15)
  {
    Log("Pinscape Controller USB error reading from device: not all bytes received");
    return true;
  }

  return true;
}

void PinscapeDevice::AllOff() { SpecialRequest(5); }

bool PinscapeDevice::SpecialRequest(uint8_t id)
{
  uint8_t buffer[9] = {0};
  buffer[0] = 0x00;
  buffer[1] = 0x41;
  buffer[2] = id;

  return WriteUSB(buffer);
}

bool PinscapeDevice::WriteUSB(uint8_t* pBuffer)
{
  int actual = hid_write(m_pDevice, pBuffer, 9);

  if (actual != 9)
  {
    Log("Pinscape Controller USB error sending request: not all bytes sent");
    return false;
  }

  return true;
}

}  // namespace DOF