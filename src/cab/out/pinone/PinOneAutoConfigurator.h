#pragma once

#include "../IAutoConfigOutputController.h"

namespace DOF
{

class Cabinet;

class PinOneAutoConfigurator : public IAutoConfigOutputController
{
public:
   virtual void AutoConfig(Cabinet* cabinet) override;
   static std::string GetDevice();
};

}