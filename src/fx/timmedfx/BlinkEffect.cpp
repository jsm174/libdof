#include "BlinkEffect.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"
#include "../../general/MathExtensions.h"
#include "../../general/StringExtensions.h"
#include "../../Log.h"
#include <climits>

namespace DOF
{

BlinkEffect::BlinkEffect()
   : m_activeValue(-1)
   , m_lowValue(0)
   , m_durationActiveMs(500)
   , m_durationInactiveMs(500)
   , m_untriggerBehaviour(BlinkEffectUntriggerBehaviourEnum::Immediate)
   , m_active(false)
   , m_blinkEnabled(false)
   , m_blinkState(false)
   , m_blinkOrgTableElementDataValue(1)
   , m_blinkTableElementData(nullptr)
{
   m_alarmCallback = [this]()
   {
      Log::Debug(StringExtensions::Build("BlinkEffect alarm callback fired for effect {0}", StringExtensions::ToAddressString(this)));
      this->DoBlink();
   };
}

BlinkEffect::~BlinkEffect()
{
   if (m_blinkTableElementData)
   {
      delete m_blinkTableElementData;
      m_blinkTableElementData = nullptr;
   }
}

void BlinkEffect::SetHighValue(int value) { m_activeValue = MathExtensions::Limit(value, -1, 255); }

void BlinkEffect::SetLowValue(int value) { m_lowValue = MathExtensions::Limit(value, 0, 255); }

void BlinkEffect::SetDurationActiveMs(int value) { m_durationActiveMs = MathExtensions::Limit(value, 1, INT_MAX); }

void BlinkEffect::SetDurationInactiveMs(int value) { m_durationInactiveMs = MathExtensions::Limit(value, 1, INT_MAX); }

void BlinkEffect::StartBlinking(TableElementData* tableElementData)
{
   Log::Debug(StringExtensions::Build("BlinkEffect::StartBlinking called on effect {0}", StringExtensions::ToAddressString(this)));

   if (m_blinkTableElementData)
      delete m_blinkTableElementData;

   m_blinkTableElementData = new TableElementData(tableElementData->m_tableElementType, tableElementData->m_number, tableElementData->m_value);
   m_blinkOrgTableElementDataValue = m_blinkTableElementData->m_value;

   if (!m_blinkEnabled)
   {
      Log::Debug(StringExtensions::Build("BlinkEffect::StartBlinking enabling blink, calling DoBlink on effect {0}", StringExtensions::ToAddressString(this)));
      m_blinkEnabled = true;
      m_blinkState = false;
      DoBlink();
      Log::Debug(StringExtensions::Build("BlinkEffect::StartBlinking DoBlink returned on effect {0}", StringExtensions::ToAddressString(this)));
   }
   else
   {
      Log::Debug(StringExtensions::Build("BlinkEffect::StartBlinking already enabled on effect {0}", StringExtensions::ToAddressString(this)));
      if (m_blinkState)
      {
         m_blinkTableElementData->m_value = (m_activeValue >= 0 ? m_activeValue : m_blinkOrgTableElementDataValue);
         TriggerTargetEffect(m_blinkTableElementData);
      }
   }
}

void BlinkEffect::StopBlinking()
{
   Log::Debug(StringExtensions::Build("BlinkEffect::StopBlinking called on effect {0} {1}", StringExtensions::ToAddressString(this), GetName()));

   m_blinkEnabled = false;
   if (m_untriggerBehaviour == BlinkEffectUntriggerBehaviourEnum::Immediate)
   {
      m_table->GetPinball()->GetAlarms()->UnregisterAlarmsForEffect(this);
      if (m_blinkTableElementData)
      {
         m_blinkTableElementData->m_value = 0;
         TriggerTargetEffect(m_blinkTableElementData);
      }
   }
}

void BlinkEffect::DoBlink()
{
   Log::Debug(StringExtensions::Build("BlinkEffect::DoBlink called on effect {0}", StringExtensions::ToAddressString(this)));

   m_blinkState = !m_blinkState;
   if (m_blinkState)
   {
      Log::Debug(StringExtensions::Build(
         "BlinkEffect::DoBlink state=active, setting value and registering alarm {0}ms on effect {1}", std::to_string(m_durationActiveMs), StringExtensions::ToAddressString(this)));
      m_blinkTableElementData->m_value = (m_activeValue >= 0 ? m_activeValue : m_blinkOrgTableElementDataValue);
      m_table->GetPinball()->GetAlarms()->RegisterAlarmForEffect(m_durationActiveMs, m_alarmCallback, this);
   }
   else
   {
      Log::Debug(StringExtensions::Build(
         "BlinkEffect::DoBlink state=inactive, registering alarm {0}ms on effect {1}", std::to_string(m_durationInactiveMs), StringExtensions::ToAddressString(this)));
      if (m_blinkEnabled)
      {
         m_blinkTableElementData->m_value = m_lowValue;
         m_table->GetPinball()->GetAlarms()->RegisterAlarmForEffect(m_durationInactiveMs, m_alarmCallback, this);
      }
      else
         m_blinkTableElementData->m_value = 0;
   }
   Log::Debug(StringExtensions::Build("BlinkEffect::DoBlink calling TriggerTargetEffect on effect {0}", StringExtensions::ToAddressString(this)));
   TriggerTargetEffect(m_blinkTableElementData);
   Log::Debug(StringExtensions::Build("BlinkEffect::DoBlink TriggerTargetEffect returned on effect {0}", StringExtensions::ToAddressString(this)));
}

void BlinkEffect::Trigger(TableElementData* tableElementData)
{
   Log::Debug(StringExtensions::Build("BlinkEffect::Trigger called with value={0}, target='{1}', element={2}", std::to_string(tableElementData->m_value),
      m_targetEffect ? m_targetEffect->GetName() : "NULL", std::to_string(tableElementData->m_number)));
   if (m_targetEffect != nullptr)
   {
      if (tableElementData->m_value != 0)
         StartBlinking(tableElementData);
      else
         StopBlinking();
   }
   else
   {
      Log::Debug("BlinkEffect::Trigger: No target effect set!");
   }
}

void BlinkEffect::Finish()
{
   Log::Debug(StringExtensions::Build("BlinkEffect::Finish called on effect {0}", StringExtensions::ToAddressString(this)));
   try
   {
      m_table->GetPinball()->GetAlarms()->UnregisterAlarmsForEffect(this);
   }
   catch (...)
   {
   }
   EffectEffectBase::Finish();
}

}