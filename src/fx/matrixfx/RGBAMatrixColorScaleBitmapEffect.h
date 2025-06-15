#pragma once

#include "MatrixColorScaleEffectBase.h"
#include "../../general/color/RGBAColor.h"

namespace DOF
{

class RGBAMatrixColorScaleBitmapEffect : public MatrixColorScaleEffectBase<RGBAColor>
{
public:
   RGBAMatrixColorScaleBitmapEffect();
   virtual ~RGBAMatrixColorScaleBitmapEffect() = default;

   const RGBAColor& GetInactiveColor() const { return m_inactiveColor; }
   void SetInactiveColor(const RGBAColor& value) { m_inactiveColor = value; }

protected:
   virtual RGBAColor GetInactiveValue() override;
   virtual RGBAColor GetColorScaledPixelValue(const PixelData& pixel, int triggerValue) override;

private:
   RGBAColor m_inactiveColor;
};

}