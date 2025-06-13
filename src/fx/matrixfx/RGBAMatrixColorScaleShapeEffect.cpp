#include "RGBAMatrixColorScaleShapeEffect.h"
#include "../../general/MathExtensions.h"

namespace DOF
{

RGBAMatrixColorScaleShapeEffect::RGBAMatrixColorScaleShapeEffect()
   : RGBAMatrixShapeEffect()
   , m_colorScale(255, 255, 255, 255)
   , m_colorScaleMode(ColorScaleMode::Multiply)
   , m_colorScaleStrength(1.0)
{
}


RGBAColor RGBAMatrixColorScaleShapeEffect::ApplyColorScaling(const RGBAColor& baseColor) const
{
   switch (m_colorScaleMode)
   {
   case ColorScaleMode::Multiply: return m_colorScale.ApplyScale(baseColor);

   case ColorScaleMode::Tint: return ColorScale::TintColor(baseColor, m_colorScale.GetColor(), m_colorScaleStrength);

   case ColorScaleMode::Replace:
   {
      double brightness = (baseColor.GetRed() * 0.299 + baseColor.GetGreen() * 0.587 + baseColor.GetBlue() * 0.114) / 255.0;
      RGBAColor scaledColor = ColorScale::ScaleBrightness(m_colorScale.GetColor(), brightness);
      return ColorScale::BlendColors(baseColor, scaledColor, m_colorScaleStrength);
   }

   default: return baseColor;
   }
}

}