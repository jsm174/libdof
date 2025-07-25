#include "AnalogAlphaMatrixBitmapEffect.h"
#include "../../general/MathExtensions.h"

namespace DOF
{

AnalogAlphaMatrixBitmapEffect::AnalogAlphaMatrixBitmapEffect()
   : m_inactiveValue(0, 0)
{
}

AnalogAlpha AnalogAlphaMatrixBitmapEffect::GetInactiveValue() { return m_inactiveValue; }

AnalogAlpha AnalogAlphaMatrixBitmapEffect::GetPixelValue(const PixelData& pixel, int triggerValue)
{
   AnalogAlpha d;

   int v = MathExtensions::Limit(triggerValue, 0, 255);

   int grayscale = (int)(0.299 * pixel.red + 0.587 * pixel.green + 0.114 * pixel.blue);
   d.SetValue(MathExtensions::Limit((int)((float)grayscale * v / 255), 0, 255));
   d.SetAlpha(MathExtensions::Limit((int)((float)pixel.alpha * v / 255), 0, 255));

   return d;
}

}