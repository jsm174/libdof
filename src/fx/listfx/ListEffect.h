#pragma once

#include "../EffectBase.h"
#include "../AssignedEffectList.h"

namespace DOF
{

class Table;
class TableElementData;

class ListEffect : public EffectBase
{
public:
   ListEffect();
   virtual ~ListEffect() = default;

   AssignedEffectList& GetAssignedEffects() { return m_assignedEffects; }
   const AssignedEffectList& GetAssignedEffects() const { return m_assignedEffects; }
   void SetAssignedEffects(const AssignedEffectList& value) { m_assignedEffects = value; }

   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Init(Table* table) override;
   virtual void Finish() override;

private:
   AssignedEffectList m_assignedEffects;
};

}