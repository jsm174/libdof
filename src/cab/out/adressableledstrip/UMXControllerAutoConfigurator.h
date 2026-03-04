#pragma once

#include "../IAutoConfigOutputController.h"
#include "UMXDevice.h"
#include <vector>

namespace DOF
{

class Cabinet;

class UMXControllerAutoConfigurator : public IAutoConfigOutputController
{
public:
   virtual void AutoConfig(Cabinet* cabinet) override;

   static void AddUMXDevice(UMXDevice* dev);
   static void RemoveAllUMXDevices();
   static std::vector<UMXDevice*> AllDevices();
   static void ClearDevices();

private:
   static std::vector<UMXDevice*> s_devices;
   static bool s_inited;
   static Cabinet* s_cabinet;
};

}
