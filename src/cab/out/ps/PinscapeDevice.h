#pragma once

#include "DOF/DOF.h"
#include <hidapi/hidapi.h>

namespace DOF
{

class PinscapeDevice
{
 public:
  PinscapeDevice(hid_device* pDevice, const std::string& szPath, const std::string& szName, unsigned short vendorID,
                 unsigned short productID, unsigned short version);
  ~PinscapeDevice();

  const std::string ToString() const { return m_szName + " (unit " + std::to_string(GetUnitNo()) + ")"; }
  int GetUnitNo() const { return m_unitNo; }
  int GetNumOutputs() const { return m_numOutputs; }

  bool IsLedWizEmulator(int unitNum);
  bool ReadUSB(uint8_t* buf);
  void AllOff();
  bool SpecialRequest(uint8_t id);
  bool WriteUSB(uint8_t* buf);

  void UpdateOutputs(uint8_t* NewOutputValues);

 private:
  hid_device* m_pDevice;
  std::string m_szPath;
  std::string m_szName;
  ushort m_vendorID;
  ushort m_productID;
  short m_version;
  bool m_plungerEnabled;
  int m_numOutputs;
  int m_unitNo;
};

}  // namespace DOF
