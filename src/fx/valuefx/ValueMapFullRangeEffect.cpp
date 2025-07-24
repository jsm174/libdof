#include "ValueMapFullRangeEffect.h"
#include "../../table/TableElementData.h"

namespace DOF
{

void ValueMapFullRangeEffect::Trigger(TableElementData* tableElementData)
{
   // Convert the element data from a PWM level to a boolean value: PWM
   // level 0 is bool OFF, any non-zero PWM level is bool ON.  We
   // represent the bool values as 0/255 for OFF/ON.
   tableElementData->m_value = (tableElementData->m_value == 0 ? 0 : 255);

   // check to see if the converted boolean value has changed since
   // the last update
   std::string key = std::to_string(static_cast<int>(tableElementData->m_tableElementType)) + std::to_string(tableElementData->m_number);

   auto it = m_previousState.find(key);
   if (it == m_previousState.end() || tableElementData->m_value != it->second)
   {
      // trigger the effect
      TriggerTargetEffect(tableElementData);

      // remember the update
      m_previousState[key] = tableElementData->m_value;
   }
}

}