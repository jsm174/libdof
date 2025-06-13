#include "MinDurationEffect.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"

namespace DOF
{

MinDurationEffect::MinDurationEffect()
   : m_retriggerBehaviour(RetriggerBehaviourEnum::Restart)
   , m_minDurationMs(500)
   , m_active(false)
   , m_untriggered(false)
   , m_durationTimerTableElementData(nullptr)
{
}

void MinDurationEffect::Trigger(TableElementData* tableElementData)
{
   if (m_targetEffect != nullptr)
   {
      if (tableElementData->m_value != 0)
      {
         if (!m_active)
         {
            TriggerTargetEffect(tableElementData);
            m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_minDurationMs, [this, tableElementData]() { this->MinDurationReached(tableElementData); }, true);
            m_active = true;
            m_untriggered = false;
            m_durationTimerTableElementData = tableElementData;
         }
         else if (m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
         {
            TriggerTargetEffect(tableElementData);
            m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_minDurationMs, [this, tableElementData]() { this->MinDurationReached(tableElementData); }, true);
            m_durationTimerTableElementData = tableElementData;
         }
      }
      else
      {
         if (m_active)
            m_untriggered = true;
      }
   }
}

void MinDurationEffect::MinDurationReached(TableElementData* tableElementData)
{
   m_active = false;
   if (m_untriggered)
   {
      tableElementData->m_value = 0;
      TriggerTargetEffect(tableElementData);
   }
   m_untriggered = false;
   m_durationTimerTableElementData = nullptr;
}

void MinDurationEffect::Finish()
{
   try
   {
   }
   catch (...)
   {
   }
   m_active = false;
   m_untriggered = false;
   m_durationTimerTableElementData = nullptr;
   EffectEffectBase::Finish();
}

}