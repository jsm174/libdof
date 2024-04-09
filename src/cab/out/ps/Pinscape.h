#pragma once

#include <hidapi/hidapi.h>

#include "../OutputControllerFlexCompleteBase.h"
#include "DOF/DOF.h"

namespace DOF
{

class PinscapeDevice;

class Pinscape : public OutputControllerFlexCompleteBase
{
 public:
  static void Initialize();

  Pinscape();
  Pinscape(int number);
  ~Pinscape();

  int GetNumber() { return m_number; }
  void SetNumber(int value);
  void SetMinCommandIntervalMs(int value);
  void Init(Cabinet* pCabinet) override;
  void Finish() override;
  bool VerifySettings() { return true; }
  void UpdateOutputs(uint8_t* pNewOutputValues);
  void UpdateDelay();
  void ConnectToController() {}
  void DisconnectFromController() {}
  static void FindDevices();
  static std::vector<PinscapeDevice*> GetAllDevices() { return m_devices; }

 private:
  static std::string GetProductName(hid_device_info* dev);

  int m_number;
  int m_minCommandIntervalMs;
  bool m_minCommandIntervalMsSet;
  uint8_t m_oldOutputValues[32];
  std::chrono::steady_clock::time_point m_lastUpdate;
  static std::vector<PinscapeDevice*> m_devices;
  PinscapeDevice* m_pDevice;
};

}  // namespace DOF
