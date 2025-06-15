#pragma once

#include "ILayerToy.h"
#include "../../general/color/RGBAColor.h"

namespace DOF
{

class IRGBAToy : public virtual ILayerToy<RGBAColor>
{
public:
   virtual ~IRGBAToy() { }

   virtual int GetOutputCount() const = 0;
   virtual void UpdateOutputs() = 0;
};

}