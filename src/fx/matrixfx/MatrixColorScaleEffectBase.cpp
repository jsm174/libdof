#include "MatrixColorScaleEffectBase.h"
#include "../../general/color/RGBAColor.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

template class MatrixColorScaleEffectBase<RGBAColor>;
template class MatrixColorScaleEffectBase<AnalogAlpha>;

}