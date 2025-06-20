#pragma once

#include "DOF/DOF.h"
#include "../Output.h"

namespace DOF
{

class LedStripOutput : public Output
{
public:
   LedStripOutput() = default;
   virtual ~LedStripOutput() = default;
};

}