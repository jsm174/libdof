#include "EffectFactory.h"


#include "rgbafx/RGBAColorEffect.h"
#include "analogtoyfx/AnalogToyValueEffect.h"
#include "timmedfx/BlinkEffect.h"
#include "timmedfx/DurationEffect.h"
#include "conditionfx/TableElementConditionEffect.h"
#include "matrixfx/RGBAMatrixShapeEffect.h"
#include "matrixfx/RGBAMatrixColorScaleShapeEffect.h"

namespace DOF
{


REGISTER_EFFECT(RGBAColorEffect, "RGBAColorEffect")
REGISTER_EFFECT(AnalogToyValueEffect, "AnalogToyValueEffect")
REGISTER_EFFECT(BlinkEffect, "BlinkEffect")
REGISTER_EFFECT(DurationEffect, "DurationEffect")
REGISTER_EFFECT(TableElementConditionEffect, "TableElementConditionEffect")


REGISTER_MATRIX_EFFECT(RGBAMatrixShapeEffect, "RGBAMatrixShapeEffect")
REGISTER_MATRIX_EFFECT(RGBAMatrixColorScaleShapeEffect, "RGBAMatrixColorScaleShapeEffect")

}