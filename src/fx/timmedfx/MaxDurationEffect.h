#pragma once

#include "../EffectEffectBase.h"
#include "../RetriggerBehaviourEnum.h"

namespace DOF
{

class TableElementData;

class MaxDurationEffect : public EffectEffectBase
{
public:
   MaxDurationEffect();
   virtual ~MaxDurationEffect() = default;

   RetriggerBehaviourEnum GetRetriggerBehaviour() const { return m_retriggerBehaviour; }
   void SetRetriggerBehaviour(RetriggerBehaviourEnum value) { m_retriggerBehaviour = value; }
   int GetMaxDurationMs() const { return m_maxDurationMs; }
   void SetMaxDurationMs(int value) { m_maxDurationMs = value; }
   bool GetActive() const { return m_active; }
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Finish() override;

private:
   void MaxDurationReached(TableElementData* tableElementData);

   RetriggerBehaviourEnum m_retriggerBehaviour;
   int m_maxDurationMs;
   bool m_active;
};

}