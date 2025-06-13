#pragma once

#include "DOF/DOF.h"
#include "IToy.h"

namespace DOF
{

class IToyUpdatable : public virtual IToy
{
public:
   virtual void UpdateToy() = 0;
   virtual void UpdateOutputs() = 0;
};

}
