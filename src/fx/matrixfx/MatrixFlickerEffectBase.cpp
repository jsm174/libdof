#include "MatrixFlickerEffectBase.h"
#include "../../general/color/RGBAColor.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

template class MatrixFlickerEffectBase<RGBAColor>;
template class MatrixFlickerEffectBase<AnalogAlpha>;

}