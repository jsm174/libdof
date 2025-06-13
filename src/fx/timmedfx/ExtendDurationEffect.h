#pragma once

#include "../EffectEffectBase.h"
#include "../RetriggerBehaviourEnum.h"

namespace DOF
{

class TableElementData;

class ExtendDurationEffect : public EffectEffectBase
{
public:
   ExtendDurationEffect();
   virtual ~ExtendDurationEffect() = default;

   RetriggerBehaviourEnum GetRetriggerBehaviour() const { return m_retriggerBehaviour; }
   void SetRetriggerBehaviour(RetriggerBehaviourEnum value) { m_retriggerBehaviour = value; }
   int GetDurationMs() const { return m_durationMs; }
   void SetDurationMs(int value) { m_durationMs = value; }
   bool GetActive() const { return m_active; }
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Finish() override;

private:
   void ExtendDurationEnd(TableElementData* tableElementData);

   RetriggerBehaviourEnum m_retriggerBehaviour;
   int m_durationMs;
   bool m_active;
};

}