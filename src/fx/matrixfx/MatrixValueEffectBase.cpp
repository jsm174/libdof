#include "MatrixValueEffectBase.h"
#include "../../general/color/RGBAColor.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

template class MatrixValueEffectBase<RGBAColor>;
template class MatrixValueEffectBase<AnalogAlpha>;

}