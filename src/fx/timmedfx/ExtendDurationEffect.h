#pragma once

#include "../EffectEffectBase.h"
#include "../RetriggerBehaviourEnum.h"
#include "../../table/TableElementData.h"

namespace DOF
{

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
   virtual std::string GetXmlElementName() const override { return "ExtendDurationEffect"; }

private:
   void ExtendDurationEnd(TableElementData* tableElementData);

   RetriggerBehaviourEnum m_retriggerBehaviour;
   int m_durationMs;
   bool m_active;
   TableElementData m_durationTimerTableElementData;
};

}