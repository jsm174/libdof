#include "MatrixEffectBase.h"
#include "../../general/color/RGBAColor.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

template class MatrixEffectBase<RGBAColor>;
template class MatrixEffectBase<AnalogAlpha>;

}