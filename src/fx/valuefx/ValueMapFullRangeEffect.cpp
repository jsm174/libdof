#include "ValueMapFullRangeEffect.h"
#include "../../table/TableElementData.h"
#include "../../general/StringExtensions.h"
#include "../../Log.h"

namespace DOF
{

void ValueMapFullRangeEffect::Trigger(TableElementData* tableElementData)
{
   tableElementData->m_value = (tableElementData->m_value == 0 ? 0 : 255);

   std::string key = StringExtensions::Build("{0}{1}", std::to_string(static_cast<int>(tableElementData->m_tableElementType)), std::to_string(tableElementData->m_number));

   auto it = m_previousState.find(key);
   if (it == m_previousState.end() || tableElementData->m_value != it->second)
   {
      TriggerTargetEffect(tableElementData);
      m_previousState[key] = tableElementData->m_value;
   }
}

}