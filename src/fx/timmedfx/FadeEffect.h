#pragma once

#include "../EffectEffectBase.h"
#include "../RetriggerBehaviourEnum.h"

#include <chrono>

namespace DOF
{

class TableElementData;

class FadeEffect : public EffectEffectBase
{
public:
   FadeEffect();
   virtual ~FadeEffect();

   RetriggerBehaviourEnum GetRetriggerBehaviour() const { return m_retriggerBehaviour; }
   void SetRetriggerBehaviour(RetriggerBehaviourEnum value) { m_retriggerBehaviour = value; }
   int GetFadeUpDurationMs() const { return m_fadeUpDurationMs; }
   void SetFadeUpDurationMs(int value) { m_fadeUpDurationMs = value; }
   int GetFadeDownDurationMs() const { return m_fadeDownDurationMs; }
   void SetFadeDownDurationMs(int value) { m_fadeDownDurationMs = value; }
   int GetFadeRefreshIntervalMs() const { return m_fadeRefreshIntervalMs; }
   void SetFadeRefreshIntervalMs(int value) { m_fadeRefreshIntervalMs = value; }
   bool GetActive() const { return m_active; }
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Finish() override;

   virtual std::string GetXmlElementName() const override { return "FadeEffect"; }

private:
   void FadeStep();
   void StartFadeUp(TableElementData* tableElementData);
   void StartFadeDown();

   RetriggerBehaviourEnum m_retriggerBehaviour;
   int m_fadeUpDurationMs;
   int m_fadeDownDurationMs;
   int m_fadeRefreshIntervalMs;
   bool m_active;
   bool m_fadingUp;
   bool m_fadingDown;
   int m_targetValue;
   int m_currentValue;
   int m_fadeStartValue;
   double m_fadeStepValue;
   TableElementData* m_fadeTableElementData;
   std::chrono::steady_clock::time_point m_fadeStartTime;
};

}