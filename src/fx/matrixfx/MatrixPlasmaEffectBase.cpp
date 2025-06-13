#include "MatrixPlasmaEffectBase.h"
#include "../../general/color/RGBAColor.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

template class MatrixPlasmaEffectBase<RGBAColor>;
template class MatrixPlasmaEffectBase<AnalogAlpha>;

}