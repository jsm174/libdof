#include "DelayEffect.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"
#include "../../general/MathExtensions.h"
#include <climits>

namespace DOF
{

DelayEffect::DelayEffect()
   : m_delayMs(0)
{
}

void DelayEffect::SetDelayMs(int value) { m_delayMs = MathExtensions::Limit(value, 0, INT_MAX); }

void DelayEffect::Trigger(TableElementData* tableElementData)
{
   if (m_targetEffect != nullptr)
   {
      if (m_delayMs > 0)
      {
         m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_delayMs, [this, tableElementData]() { this->AfterDelay(tableElementData); }, true);
      }
      else
      {
         TriggerTargetEffect(tableElementData);
      }
   }
}

void DelayEffect::AfterDelay(TableElementData* data) { TriggerTargetEffect(data); }

void DelayEffect::Init(Table* table) { EffectEffectBase::Init(table); }

void DelayEffect::Finish()
{
   try
   {
   }
   catch (...)
   {
   }
   EffectEffectBase::Finish();
}

}