#include "RGBAMatrixColorScaleBitmapEffect.h"
#include "../../general/MathExtensions.h"

namespace DOF
{

RGBAMatrixColorScaleBitmapEffect::RGBAMatrixColorScaleBitmapEffect()
   : m_inactiveColor(0, 0, 0, 0)
{
}

RGBAColor RGBAMatrixColorScaleBitmapEffect::GetInactiveValue() { return m_inactiveColor; }

RGBAColor RGBAMatrixColorScaleBitmapEffect::GetColorScaledPixelValue(const PixelData& pixel, int triggerValue)
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

}