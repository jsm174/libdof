#include "ExtendDurationEffect.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"

namespace DOF
{

ExtendDurationEffect::ExtendDurationEffect()
   : m_retriggerBehaviour(RetriggerBehaviourEnum::Restart)
   , m_durationMs(500)
   , m_active(false)
   , m_durationTimerTableElementData()
{
}

void ExtendDurationEffect::Trigger(TableElementData* tableElementData)
{
   if (m_targetEffect != nullptr)
   {
      TriggerTargetEffect(tableElementData);

      if (tableElementData->m_value != 0)
      {
         if (!m_active)
         {
            m_durationTimerTableElementData = *tableElementData;
            m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_durationMs, [this]() { this->ExtendDurationEnd(&m_durationTimerTableElementData); }, true);
            m_active = true;
         }
         else if (m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
         {
            m_durationTimerTableElementData = *tableElementData;
            m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_durationMs, [this]() { this->ExtendDurationEnd(&m_durationTimerTableElementData); }, true);
         }
      }
   }
}

void ExtendDurationEffect::ExtendDurationEnd(TableElementData* tableElementData)
{
   tableElementData->m_value = 0;
   TriggerTargetEffect(tableElementData);
   m_active = false;
}

void ExtendDurationEffect::Finish()
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