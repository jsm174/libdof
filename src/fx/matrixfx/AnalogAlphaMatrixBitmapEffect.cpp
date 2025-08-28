#include "AnalogAlphaMatrixBitmapEffect.h"

namespace DOF
{

AnalogAlpha AnalogAlphaMatrixBitmapEffect::GetEffectValue(int triggerValue, PixelData pixel)
{
   return AnalogAlpha((pixel.red + pixel.green + pixel.blue) / 3, (int)((float)pixel.alpha * triggerValue / 255));
}

}