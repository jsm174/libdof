#pragma once

#include "DOF/DOF.h"
#include "hidapi.h"

namespace DOF
{

class PinscapeDevice;

class Pinscape
{
 public:
  Pinscape();
  ~Pinscape();

  void FindDevices();
  void DataReceive(char type, int number, int value);

 private:
  std::string GetProductName(hid_device_info* dev);

  std::vector<PinscapeDevice*> m_devices;
};

}  // namespace DOF
