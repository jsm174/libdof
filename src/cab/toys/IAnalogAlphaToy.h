#pragma once

#include "ILayerToy.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

class IAnalogAlphaToy : public virtual ILayerToy<AnalogAlpha>
{
public:
   virtual ~IAnalogAlphaToy() { }

   virtual int GetOutputCount() const = 0;
   virtual void UpdateOutputs() = 0;
};

}