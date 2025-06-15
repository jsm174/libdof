#include "MatrixBitmapAnimationEffectBase.h"
#include "../../general/color/RGBAColor.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{
template class MatrixBitmapAnimationEffectBase<RGBAColor>;
template class MatrixBitmapAnimationEffectBase<AnalogAlpha>;

}