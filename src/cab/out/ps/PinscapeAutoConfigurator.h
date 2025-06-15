#pragma once

#include "../IAutoConfigOutputController.h"

namespace DOF
{

class Cabinet;
class Pinscape;

class PinscapeAutoConfigurator : public IAutoConfigOutputController
{
public:
   PinscapeAutoConfigurator() { }
   ~PinscapeAutoConfigurator() { }

   void AutoConfig(Cabinet* pCabinet) override;
};

}
