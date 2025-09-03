#pragma once

#include "../IAutoConfigOutputController.h"

namespace DOF
{

class Cabinet;

class PacUIOAutoConfigurator : public IAutoConfigOutputController
{
public:
   PacUIOAutoConfigurator() = default;
   virtual ~PacUIOAutoConfigurator() = default;

   virtual void AutoConfig(Cabinet* cabinet) override;
};

}