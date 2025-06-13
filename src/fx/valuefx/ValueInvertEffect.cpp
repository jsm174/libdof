#include "ValueInvertEffect.h"
#include "../../table/TableElementData.h"

namespace DOF
{

void ValueInvertEffect::Trigger(TableElementData* tableElementData)
{
   tableElementData->m_value = 255 - tableElementData->m_value;
   TriggerTargetEffect(tableElementData);
}

}