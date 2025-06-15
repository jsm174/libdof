#pragma once

#include "../IAutoConfigOutputController.h"

namespace DOF
{

class LedWizAutoConfigurator : public IAutoConfigOutputController
{
public:
   LedWizAutoConfigurator();
   virtual ~LedWizAutoConfigurator();

   virtual void AutoConfig(Cabinet* pCabinet) override;
};

}