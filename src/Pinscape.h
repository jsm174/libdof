#pragma once

#include <chrono>

#include "DOF/DOF.h"
#include "hidapi.h"

namespace DOF
{

class PinscapeDevice;

class Pinscape
{
 public:
  static void Initialize();

  Pinscape();
  Pinscape(int number);
  ~Pinscape();

  void SetNumber(int value);
  void SetMinCommandIntervalMs(int value);
  void Init();
  void Finish();
  bool VerifySettings() { return true; }
  void UpdateOutputs(uint8_t* pNewOutputValues);
  void UpdateDelay();
  void ConnectToController() {}
  void DisconnectFromController() {}
  static void FindDevices();

 private:
  static std::string GetProductName(hid_device_info* dev);

  int m_number;
  int m_minCommandIntervalMs;
  bool m_minCommandIntervalMsSet;
  uint8_t m_oldOutputValues[32];
  std::chrono::steady_clock::time_point m_lastUpdate;
  static std::vector<PinscapeDevice*> m_devices;
  PinscapeDevice* m_pDevice;

  std::string m_szName;
  int m_numberOfOutputs;
};

}  // namespace DOF
