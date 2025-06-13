#include "RGBAMatrixBitmapEffect.h"
#include "../../general/MathExtensions.h"

namespace DOF
{

RGBAMatrixBitmapEffect::RGBAMatrixBitmapEffect()
   : m_inactiveColor(0, 0, 0, 0)
{
}

RGBAColor RGBAMatrixBitmapEffect::GetInactiveValue() { return m_inactiveColor; }

RGBAColor RGBAMatrixBitmapEffect::GetPixelValue(const PixelData& pixel, int triggerValue)
{
   RGBAColor d;

   int v = MathExtensions::Limit(triggerValue, 0, 255);

   d.SetRed(MathExtensions::Limit((int)((float)pixel.red * v / 255), 0, 255));
   d.SetGreen(MathExtensions::Limit((int)((float)pixel.green * v / 255), 0, 255));
   d.SetBlue(MathExtensions::Limit((int)((float)pixel.blue * v / 255), 0, 255));
   d.SetAlpha(MathExtensions::Limit((int)((float)pixel.alpha * v / 255), 0, 255));

   return d;
}

}