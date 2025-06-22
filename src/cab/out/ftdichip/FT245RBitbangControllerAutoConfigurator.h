#pragma once

#include "../IAutoConfigOutputController.h"
#include <string>

namespace DOF
{

class Cabinet;

class FT245RBitbangControllerAutoConfigurator : public IAutoConfigOutputController
{
public:
   FT245RBitbangControllerAutoConfigurator() = default;
   virtual ~FT245RBitbangControllerAutoConfigurator() = default;

   virtual void AutoConfig(Cabinet* cabinet) override;

private:
   struct DeviceInfo
   {
      DeviceInfo(const std::string& serialNumber, const std::string& description)
         : serial(serialNumber)
         , desc(description)
      {
      }

      std::string serial;
      std::string desc;
   };
};

}