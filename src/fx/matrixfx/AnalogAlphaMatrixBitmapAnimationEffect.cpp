#include "AnalogAlphaMatrixBitmapAnimationEffect.h"

namespace DOF
{

AnalogAlpha AnalogAlphaMatrixBitmapAnimationEffect::GetEffectValue(int triggerValue, PixelData pixel)
{
   return AnalogAlpha((pixel.red + pixel.green + pixel.blue) / 3, (int)((float)pixel.alpha * triggerValue / 255));
}

}