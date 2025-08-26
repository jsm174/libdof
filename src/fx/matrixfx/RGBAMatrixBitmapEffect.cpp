#include "RGBAMatrixBitmapEffect.h"

namespace DOF
{

RGBAColor RGBAMatrixBitmapEffect::GetEffectValue(int triggerValue, PixelData pixel)
{
   RGBAColor d = pixel.GetRGBAColor();
   d.SetAlpha((int)((float)pixel.alpha * triggerValue / 255));
   return d;
}

}