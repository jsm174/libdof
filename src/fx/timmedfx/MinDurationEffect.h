#pragma once

#include "../EffectEffectBase.h"
#include "../RetriggerBehaviourEnum.h"
#include "../../table/TableElementData.h"
#include <chrono>

namespace DOF
{

class MinDurationEffect : public EffectEffectBase
{
public:
   MinDurationEffect();
   virtual ~MinDurationEffect() = default;

   RetriggerBehaviourEnum GetRetriggerBehaviour() const { return m_retriggerBehaviour; }
   void SetRetriggerBehaviour(RetriggerBehaviourEnum value) { m_retriggerBehaviour = value; }
   int GetMinDurationMs() const { return m_minDurationMs; }
   void SetMinDurationMs(int value) { m_minDurationMs = value; }
   bool GetActive() const { return m_active; }
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Finish() override;
   virtual std::string GetXmlElementName() const override { return "MinDurationEffect"; }

private:
   void MinDurationReached(TableElementData* tableElementData);
   void MinDurationEnd();

   RetriggerBehaviourEnum m_retriggerBehaviour;
   int m_minDurationMs;
   bool m_active;
   bool m_untriggered;
   TableElementData m_durationTimerTableElementData;
   std::chrono::steady_clock::time_point m_durationStart;
};

}