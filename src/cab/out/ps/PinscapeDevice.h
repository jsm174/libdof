#pragma once

#include <hidapi/hidapi.h>

#include "DOF/DOF.h"

namespace DOF
{

class PinscapeDevice
{
public:
   PinscapeDevice(hid_device* pDevice, const std::string& path, const std::string& name, unsigned short vendorID, unsigned short productID, unsigned short version);
   ~PinscapeDevice();

   const std::string ToString() const { return m_name + " (unit " + std::to_string(GetUnitNo()) + ")"; }
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
   std::string m_path;
   std::string m_name;
   uint16_t m_vendorID;
   uint16_t m_productID;
   short m_version;
   bool m_plungerEnabled;
   int m_numOutputs;
   int m_unitNo;
};

} // namespace DOF
