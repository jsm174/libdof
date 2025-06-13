#pragma once

#include "ToyGroupBase.h"
#include "../../../general/analog/AnalogAlpha.h"

namespace DOF
{

class AnalogAlphaToyGroup : public ToyGroupBase<AnalogAlpha>
{
public:
   AnalogAlphaToyGroup();
   virtual ~AnalogAlphaToyGroup();
};

}