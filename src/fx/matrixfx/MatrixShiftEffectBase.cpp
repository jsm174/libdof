#include "MatrixShiftEffectBase.h"
#include "../../general/color/RGBAColor.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

template class MatrixShiftEffectBase<RGBAColor>;
template class MatrixShiftEffectBase<AnalogAlpha>;

}