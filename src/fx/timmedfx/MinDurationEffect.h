#pragma once

#include "../EffectEffectBase.h"
#include "../RetriggerBehaviourEnum.h"

namespace DOF
{

class TableElementData;

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

private:
   void MinDurationReached(TableElementData* tableElementData);

   RetriggerBehaviourEnum m_retriggerBehaviour;
   int m_minDurationMs;
   bool m_active;
   bool m_untriggered;
   TableElementData* m_durationTimerTableElementData;
};

}