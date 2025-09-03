#pragma once

#include "../IAutoConfigOutputController.h"

namespace DOF
{

class Cabinet;

class PacDriveAutoConfigurator : public IAutoConfigOutputController
{
public:
   PacDriveAutoConfigurator() = default;
   virtual ~PacDriveAutoConfigurator() = default;

   virtual void AutoConfig(Cabinet* cabinet) override;
};

}