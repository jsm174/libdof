#include "MatrixBitmapEffectBase.h"
#include "../../general/color/RGBAColor.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

template class MatrixBitmapEffectBase<RGBAColor>;
template class MatrixBitmapEffectBase<AnalogAlpha>;

}