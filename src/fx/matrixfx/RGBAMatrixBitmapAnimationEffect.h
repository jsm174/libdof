#pragma once

#include "MatrixBitmapAnimationEffectBase.h"
#include "../../general/color/RGBAColor.h"

namespace DOF
{

class RGBAMatrixBitmapAnimationEffect : public MatrixBitmapAnimationEffectBase<RGBAColor>
{
public:
   RGBAMatrixBitmapAnimationEffect();
   virtual ~RGBAMatrixBitmapAnimationEffect() = default;

   const RGBAColor& GetInactiveColor() const { return m_inactiveColor; }
   void SetInactiveColor(const RGBAColor& value) { m_inactiveColor = value; }

protected:
   virtual RGBAColor GetInactiveValue() override;
   virtual RGBAColor GetPixelValue(const PixelData& pixel, int triggerValue) override;

private:
   RGBAColor m_inactiveColor;
};

}