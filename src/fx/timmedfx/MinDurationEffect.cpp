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
   , m_durationTimerTableElementData()
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
            m_durationStart = std::chrono::steady_clock::now();
            TriggerTargetEffect(tableElementData);
            m_durationTimerTableElementData = *tableElementData;
            m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_minDurationMs, [this]() { this->MinDurationReached(&m_durationTimerTableElementData); }, true);
            m_active = true;
            m_untriggered = false;
         }
         else if (m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
         {
            m_durationStart = std::chrono::steady_clock::now();
            TriggerTargetEffect(tableElementData);
            m_durationTimerTableElementData = *tableElementData;
            m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_minDurationMs, [this]() { this->MinDurationReached(&m_durationTimerTableElementData); }, true);
         }
      }
      else
      {
         if (m_active && m_durationTimerTableElementData.m_tableElementType == tableElementData->m_tableElementType && m_durationTimerTableElementData.m_number == tableElementData->m_number)
         {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_durationStart).count();

            if (elapsed >= m_minDurationMs)
            {
               MinDurationEnd();
            }
            else
            {
               int remainingMs = m_minDurationMs - static_cast<int>(elapsed);
               m_untriggered = true;
               m_table->GetPinball()->GetAlarms()->RegisterAlarm(remainingMs, [this]() { this->MinDurationEnd(); }, true);
            }
         }
      }
   }
}

void MinDurationEffect::MinDurationReached(TableElementData* tableElementData)
{
   if (m_untriggered)
   {
      MinDurationEnd();
   }
}

void MinDurationEffect::MinDurationEnd()
{
   if (m_active)
   {
      TableElementData endData = m_durationTimerTableElementData;
      endData.m_value = 0;
      TriggerTargetEffect(&endData);
   }

   m_active = false;
   m_untriggered = false;
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
   EffectEffectBase::Finish();
}

}