#pragma once

#include "MatrixBitmapAnimationEffectBase.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

class AnalogAlphaMatrixBitmapAnimationEffect : public MatrixBitmapAnimationEffectBase<AnalogAlpha>
{
public:
   virtual std::string GetXmlElementName() const override { return "AnalogAlphaMatrixBitmapAnimationEffect"; }

protected:
   virtual AnalogAlpha GetEffectValue(int triggerValue, PixelData pixel) override;
};

}