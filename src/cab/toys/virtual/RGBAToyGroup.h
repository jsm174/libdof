#pragma once

#include "ToyGroupBase.h"
#include "../../../general/color/RGBAColor.h"

namespace DOF
{

class RGBAToyGroup : public ToyGroupBase<RGBAColor>
{
public:
   RGBAToyGroup();
   virtual ~RGBAToyGroup();
};

}