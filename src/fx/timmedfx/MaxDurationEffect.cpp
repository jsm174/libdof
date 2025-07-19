#include "MaxDurationEffect.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"

namespace DOF
{

MaxDurationEffect::MaxDurationEffect()
   : m_retriggerBehaviour(RetriggerBehaviourEnum::Restart)
   , m_maxDurationMs(5000)
   , m_active(false)
   , m_durationTimerTableElementData()
{
}

void MaxDurationEffect::Trigger(TableElementData* tableElementData)
{
   if (m_targetEffect != nullptr)
   {
      if (tableElementData->m_value != 0)
      {
         if (!m_active)
         {
            TriggerTargetEffect(tableElementData);
            m_durationTimerTableElementData = *tableElementData;
            m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_maxDurationMs, [this]() { this->MaxDurationReached(&m_durationTimerTableElementData); }, true);
            m_active = true;
         }
         else if (m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
         {
            TriggerTargetEffect(tableElementData);
            m_durationTimerTableElementData = *tableElementData;
            m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_maxDurationMs, [this]() { this->MaxDurationReached(&m_durationTimerTableElementData); }, true);
         }
      }
      else
      {
         if (m_active)
         {
            TriggerTargetEffect(tableElementData);
            m_active = false;
         }
      }
   }
}

void MaxDurationEffect::MaxDurationReached(TableElementData* tableElementData)
{
   tableElementData->m_value = 0;
   TriggerTargetEffect(tableElementData);
   m_active = false;
}

void MaxDurationEffect::Finish()
{
   try
   {
   }
   catch (...)
   {
   }
   m_active = false;
   EffectEffectBase::Finish();
}

}