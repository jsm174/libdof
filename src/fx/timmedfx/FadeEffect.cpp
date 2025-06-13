#include "FadeEffect.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"
#include "../../general/MathExtensions.h"
#include "../../general/StringExtensions.h"
#include "../../Log.h"
#include <chrono>

namespace DOF
{

FadeEffect::FadeEffect()
   : m_retriggerBehaviour(RetriggerBehaviourEnum::Restart)
   , m_fadeUpDurationMs(500)
   , m_fadeDownDurationMs(500)
   , m_fadeRefreshIntervalMs(20)
   , m_active(false)
   , m_fadingUp(false)
   , m_fadingDown(false)
   , m_targetValue(0)
   , m_currentValue(0)
   , m_fadeStartValue(0)
   , m_fadeStepValue(0.0)
   , m_fadeTableElementData(nullptr)
{
}

FadeEffect::~FadeEffect()
{
   if (m_fadeTableElementData)
   {
      delete m_fadeTableElementData;
      m_fadeTableElementData = nullptr;
   }
}

void FadeEffect::Trigger(TableElementData* tableElementData)
{
   Log::Debug(StringExtensions::Build("FadeEffect::Trigger: value={0}, active={1}", std::to_string(tableElementData->m_value), std::to_string(m_active)));
   if (m_targetEffect != nullptr)
   {
      if (tableElementData->m_value != 0)
      {
         Log::Debug("FadeEffect::Trigger: Non-zero value, checking fade up");
         if (!m_active)
         {
            Log::Debug("FadeEffect::Trigger: Starting fade up (not active)");
            StartFadeUp(tableElementData);
         }
         else if (m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
         {
            Log::Debug("FadeEffect::Trigger: Restarting fade up (retrigger)");
            StartFadeUp(tableElementData);
         }
      }
      else
      {
         Log::Debug("FadeEffect::Trigger: Zero value, checking fade down");
         if (m_active)
         {
            Log::Debug("FadeEffect::Trigger: Starting fade down");
            StartFadeDown();
         }
      }
   }
}

void FadeEffect::StartFadeUp(TableElementData* tableElementData)
{
   m_fadeStartValue = m_currentValue;
   m_targetValue = tableElementData->m_value;
   if (m_fadeTableElementData)
   {
      delete m_fadeTableElementData;
   }
   m_fadeTableElementData = new TableElementData(tableElementData->m_tableElementType, tableElementData->m_number, tableElementData->m_value);
   m_active = true;
   m_fadingUp = true;
   m_fadingDown = false;
   m_fadeStartTime = std::chrono::steady_clock::now();

   if (m_fadeUpDurationMs > 0)
   {
      int steps = m_fadeUpDurationMs / m_fadeRefreshIntervalMs;
      m_fadeStepValue = (double)(m_targetValue - m_currentValue) / steps;

      m_table->GetPinball()->GetAlarms()->RegisterAlarmForEffect(m_fadeRefreshIntervalMs, [this]() { this->FadeStep(); }, this);
   }
   else
   {
      m_currentValue = m_targetValue;
      m_fadeTableElementData->m_value = m_currentValue;
      TriggerTargetEffect(m_fadeTableElementData);
      m_fadingUp = false;
   }
}

void FadeEffect::StartFadeDown()
{
   m_fadeStartValue = m_currentValue;
   m_targetValue = 0;
   m_fadingUp = false;
   m_fadingDown = true;
   m_fadeStartTime = std::chrono::steady_clock::now();

   if (m_fadeDownDurationMs > 0)
   {
      int steps = m_fadeDownDurationMs / m_fadeRefreshIntervalMs;
      m_fadeStepValue = (double)(m_targetValue - m_currentValue) / steps;

      m_table->GetPinball()->GetAlarms()->RegisterAlarmForEffect(m_fadeRefreshIntervalMs, [this]() { this->FadeStep(); }, this);
   }
   else
   {
      m_currentValue = m_targetValue;
      m_fadeTableElementData->m_value = m_currentValue;
      TriggerTargetEffect(m_fadeTableElementData);
      m_fadingDown = false;
      m_active = false;
   }
}

void FadeEffect::FadeStep()
{
   if (!m_active || m_fadeTableElementData == nullptr)
      return;

   auto now = std::chrono::steady_clock::now();
   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_fadeStartTime).count();

   bool fadeComplete = false;

   if (m_fadingUp)
   {
      if (elapsed >= m_fadeUpDurationMs)
      {
         m_currentValue = m_targetValue;
         fadeComplete = true;
         m_fadingUp = false;
      }
      else
      {
         double progress = (double)elapsed / m_fadeUpDurationMs;
         m_currentValue = (int)(m_fadeStartValue + (m_targetValue - m_fadeStartValue) * progress);
         m_currentValue = MathExtensions::Limit(m_currentValue, 0, 255);
      }
   }
   else if (m_fadingDown)
   {
      if (elapsed >= m_fadeDownDurationMs)
      {
         m_currentValue = 0;
         fadeComplete = true;
         m_fadingDown = false;
         m_active = false;
      }
      else
      {
         double progress = (double)elapsed / m_fadeDownDurationMs;
         m_currentValue = (int)(m_fadeStartValue + (m_targetValue - m_fadeStartValue) * progress);
         m_currentValue = MathExtensions::Limit(m_currentValue, 0, 255);
      }
   }

   m_fadeTableElementData->m_value = m_currentValue;
   TriggerTargetEffect(m_fadeTableElementData);

   if (!fadeComplete)
      m_table->GetPinball()->GetAlarms()->RegisterAlarmForEffect(m_fadeRefreshIntervalMs, [this]() { this->FadeStep(); }, this);
}

void FadeEffect::Finish()
{
   try
   {
      m_table->GetPinball()->GetAlarms()->UnregisterAlarmsForEffect(this);
   }
   catch (...)
   {
   }
   m_active = false;
   m_fadingUp = false;
   m_fadingDown = false;
   m_fadeTableElementData = nullptr;
   EffectEffectBase::Finish();
}

}