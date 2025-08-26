#pragma once

#include "MatrixBitmapEffectBase.h"
#include "../../general/color/RGBAColor.h"

namespace DOF
{

class RGBAMatrixBitmapEffect : public MatrixBitmapEffectBase<RGBAColor>
{
public:
   virtual std::string GetXmlElementName() const override { return "RGBAMatrixBitmapEffect"; }

   virtual RGBAColor GetEffectValue(int triggerValue, PixelData pixel) override;
};

}