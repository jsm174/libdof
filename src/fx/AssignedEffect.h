#pragma once

#include "DOF/DOF.h"
#include "IEffect.h"
#include <functional>

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

   const std::string& GetEffectName() const { return m_effectName; }
   void SetEffectName(const std::string& effectName);
   std::function<void()> m_effectNameChanged;
   IEffect* GetEffect() { return m_pEffect; }
   void SetEffect(IEffect* pEffect) { m_pEffect = pEffect; }
   void Trigger(TableElementData* tableElementData);
   void Init(Table* table);
   void Finish();

protected:
   void TableElementEffect_EffectNameChanged();

private:
   void ResolveEffectName(Table* table);

   std::string m_effectName;
   IEffect* m_pEffect;
};

}
