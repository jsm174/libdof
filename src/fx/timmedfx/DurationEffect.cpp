#include "DurationEffect.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"

namespace DOF
{

DurationEffect::DurationEffect()
   : m_retriggerBehaviour(RetriggerBehaviourEnum::Restart)
   , m_durationMs(500)
   , m_active(false)
   , m_durationTableElementData(nullptr)
{
}

void DurationEffect::Trigger(TableElementData* tableElementData)
{
   if (m_targetEffect != nullptr)
   {
      if (tableElementData->m_value != 0)
      {
         if (!m_active)
         {
            TriggerTargetEffect(tableElementData);
            m_active = true;
         }
         else if (m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
         {
            TriggerTargetEffect(tableElementData);
         }
         else
         {
            TriggerTargetEffect(tableElementData);
         }
      }
      else if (m_active)
      {
         m_durationTableElementData = new TableElementData(tableElementData->m_tableElementType, tableElementData->m_number, tableElementData->m_value);
         m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_durationMs, [this]() { this->DurationEnd(); });
      }
      else
      {
         TriggerTargetEffect(tableElementData);
      }
   }
}

void DurationEffect::DurationEnd()
{
   if (m_durationTableElementData)
   {
      m_durationTableElementData->m_value = 0;
      TriggerTargetEffect(m_durationTableElementData);
      delete m_durationTableElementData;
      m_durationTableElementData = nullptr;
   }
   m_active = false;
}

void DurationEffect::Finish()
{
   try
   {
      if (m_durationTableElementData)
      {
         delete m_durationTableElementData;
         m_durationTableElementData = nullptr;
      }
   }
   catch (...)
   {
   }
   m_active = false;
   EffectEffectBase::Finish();
}

}