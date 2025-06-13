#pragma once

#include "MatrixBitmapEffectBase.h"
#include "../../general/color/RGBAColor.h"

namespace DOF
{

class RGBAMatrixBitmapEffect : public MatrixBitmapEffectBase<RGBAColor>
{
public:
   RGBAMatrixBitmapEffect();
   virtual ~RGBAMatrixBitmapEffect() = default;

   const RGBAColor& GetInactiveColor() const { return m_inactiveColor; }
   void SetInactiveColor(const RGBAColor& value) { m_inactiveColor = value; }

protected:
   virtual RGBAColor GetInactiveValue() override;
   virtual RGBAColor GetPixelValue(const PixelData& pixel, int triggerValue) override;

private:
   RGBAColor m_inactiveColor;
};

}