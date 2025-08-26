#pragma once

#include "MatrixBitmapAnimationEffectBase.h"
#include "../../general/color/RGBAColor.h"

namespace DOF
{

class RGBAMatrixBitmapAnimationEffect : public MatrixBitmapAnimationEffectBase<RGBAColor>
{
public:
   virtual std::string GetXmlElementName() const override { return "RGBAMatrixBitmapAnimationEffect"; }

protected:
   virtual RGBAColor GetEffectValue(int triggerValue, PixelData pixel) override;
};

}