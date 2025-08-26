#pragma once

#include "MatrixBitmapEffectBase.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

class AnalogAlphaMatrixBitmapEffect : public MatrixBitmapEffectBase<AnalogAlpha>
{
public:
   virtual std::string GetXmlElementName() const override { return "AnalogAlphaMatrixBitmapEffect"; }

protected:
   virtual AnalogAlpha GetEffectValue(int triggerValue, PixelData pixel) override;
};

}