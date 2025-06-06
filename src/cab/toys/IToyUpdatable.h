#pragma once

#include "DOF/DOF.h"
#include "IToy.h"

namespace DOF
{

class IToyUpdatable : public IToy
{
public:
   virtual void UpdateOutputs() = 0;
};

} // namespace DOF
