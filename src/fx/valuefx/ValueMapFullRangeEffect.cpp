#include "ValueMapFullRangeEffect.h"
#include "../../table/TableElementData.h"
#include "../../general/StringExtensions.h"
#include "../../Log.h"

namespace DOF
{

void ValueMapFullRangeEffect::Trigger(TableElementData* tableElementData)
{
   int oldval = tableElementData->m_value;
   int newval = tableElementData->m_value = (tableElementData->m_value == 0 ? 0 : 255);

   Log::Debug(StringExtensions::Build("ValueMapFullRangeEffect::Trigger: {0}->{1}", std::to_string(oldval), std::to_string(newval)));

   std::string key = StringExtensions::Build("{0}{1}", std::to_string(static_cast<int>(tableElementData->m_tableElementType)), std::to_string(tableElementData->m_number));

   auto it = m_previousState.find(key);
   if (it == m_previousState.end() || newval != it->second)
   {
      Log::Debug(StringExtensions::Build("ValueMapFullRangeEffect::Trigger: Triggering target with value {0}", std::to_string(newval)));
      TriggerTargetEffect(tableElementData);
      m_previousState[key] = newval;
   }
   else
   {
      Log::Debug(StringExtensions::Build("ValueMapFullRangeEffect::Trigger: Skipping duplicate value {0}", std::to_string(newval)));
   }
}

}