#include "RGBAMatrixColorScaleBitmapAnimationEffect.h"
#include "../../general/MathExtensions.h"

namespace DOF
{

RGBAMatrixColorScaleBitmapAnimationEffect::RGBAMatrixColorScaleBitmapAnimationEffect()
   : m_inactiveColor(0, 0, 0, 0)
   , m_colorScale(255, 255, 255, 255)
   , m_colorScaleMode(ColorScaleMode::Multiply)
   , m_colorScaleStrength(1.0)
{
}

RGBAColor RGBAMatrixColorScaleBitmapAnimationEffect::GetInactiveValue() { return m_inactiveColor; }

RGBAColor RGBAMatrixColorScaleBitmapAnimationEffect::GetPixelValue(const PixelData& pixel, int triggerValue)
{
   RGBAColor scaledColor = ApplyColorScaling(pixel);

   int v = MathExtensions::Limit(triggerValue, 0, 255);

   RGBAColor result;
   result.SetRed(MathExtensions::Limit((int)((float)scaledColor.GetRed() * v / 255), 0, 255));
   result.SetGreen(MathExtensions::Limit((int)((float)scaledColor.GetGreen() * v / 255), 0, 255));
   result.SetBlue(MathExtensions::Limit((int)((float)scaledColor.GetBlue() * v / 255), 0, 255));
   result.SetAlpha(MathExtensions::Limit((int)((float)scaledColor.GetAlpha() * v / 255), 0, 255));

   return result;
}

RGBAColor RGBAMatrixColorScaleBitmapAnimationEffect::ApplyColorScaling(const PixelData& pixel) const
{
   RGBAColor baseColor(pixel.red, pixel.green, pixel.blue, pixel.alpha);

   switch (m_colorScaleMode)
   {
   case ColorScaleMode::Multiply: return m_colorScale.ApplyScale(baseColor);

   case ColorScaleMode::Tint: return ColorScale::TintColor(baseColor, m_colorScale.GetColor(), m_colorScaleStrength);

   case ColorScaleMode::Replace:
   {
      double brightness = (pixel.red * 0.299 + pixel.green * 0.587 + pixel.blue * 0.114) / 255.0;
      RGBAColor scaledColor = ColorScale::ScaleBrightness(m_colorScale.GetColor(), brightness);
      return ColorScale::BlendColors(baseColor, scaledColor, m_colorScaleStrength);
   }

   default: return baseColor;
   }
}

}