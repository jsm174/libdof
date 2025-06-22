#pragma once

#include "../IAutoConfigOutputController.h"

namespace DOF
{

class Cabinet;

class DudesCabAutoConfigurator : public IAutoConfigOutputController
{
public:
   virtual void AutoConfig(Cabinet* cabinet) override;
};

}