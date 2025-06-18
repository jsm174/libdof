#pragma once

#include "../IAutoConfigOutputController.h"

namespace DOF
{

class PinscapePicoAutoConfigurator : public IAutoConfigOutputController
{
public:
   void AutoConfig(Cabinet* cabinet) override;
};

}