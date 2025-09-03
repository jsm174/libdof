#pragma once

#include "../IAutoConfigOutputController.h"

namespace DOF
{

class Cabinet;

class PacLed64AutoConfigurator : public IAutoConfigOutputController
{
public:
   PacLed64AutoConfigurator() = default;
   virtual ~PacLed64AutoConfigurator() = default;

   virtual void AutoConfig(Cabinet* cabinet) override;
};

}