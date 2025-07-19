#pragma once

#include "../Cabinet.h"

namespace DOF
{

class IAutoConfigOutputController
{
public:
   IAutoConfigOutputController() { }
   virtual ~IAutoConfigOutputController() { }

   virtual void AutoConfig(Cabinet* cabinet) = 0;
};

}
