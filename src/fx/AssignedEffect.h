#pragma once

#include "DOF/DOF.h"
#include "IEffect.h"

namespace DOF
{

class Table;
class TableElementData;

class AssignedEffect
{
public:
   AssignedEffect();
   AssignedEffect(const std::string& effectName);
   ~AssignedEffect() { }

   void InitAssignedEffects(Table* pTable);
   const std::string& GetEffectName() const { return m_effectName; }
   void SetEffectName(const std::string& effectName);
   // event EventHandler<EventArgs> EffectNameChanged;
   IEffect* GetEffect() { return m_pEffect; }
   void SetEffect(IEffect* pEffect) { m_pEffect = pEffect; }
   void Trigger(TableElementData* pTableElementData);
   void Init(Table* pTable);
   void Finish();

protected:
   // void TableElementEffect_EffectNameChanged(object sender, EventArgs e)

private:
   void ResolveEffectName(Table* pTable);

   std::string m_effectName;
   IEffect* m_pEffect;
};

} // namespace DOF
