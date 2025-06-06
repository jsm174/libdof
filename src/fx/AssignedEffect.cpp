#include "AssignedEffect.h"

#include "../table/Table.h"
#include "../table/TableElementData.h"

namespace DOF
{

AssignedEffect::AssignedEffect()
{
   m_pEffect = nullptr;

   // EffectNameChanged += new EventHandler<EventArgs>(TableElementEffect_EffectNameChanged);
}

AssignedEffect::AssignedEffect(const std::string& effectName)
{
   m_pEffect = nullptr;

   // EffectNameChanged += new EventHandler<EventArgs>(TableElementEffect_EffectNameChanged);

   SetEffectName(effectName);
}

void AssignedEffect::SetEffectName(const std::string& effectName)
{
   if (m_effectName != effectName)
   {
      m_effectName = effectName;

      if (!m_effectName.empty())
      {
         // EffectNameChanged(this, new EventArgs());
      }
   }
}

void AssignedEffect::ResolveEffectName(Table* pTable)
{
   if (!m_effectName.empty()) // && pTable->GetEffects().ContainsKey(m_effectName))
   {
      // m_pEffect = pTable->GetEffects()[m_effectName];
   }
}

void AssignedEffect::Trigger(TableElementData* pTableElementData)
{
   if (m_pEffect)
   {
      // try {
      // m_pEffect->Trigger(pTableElementData);
      // } catch (Exception ex) {
      // Log::Write("A exception occured when triggering effect {0} for table element {1} {2} with value {3}.
      // Effect assignement will be deactivated.".Build(new object[] { Effect.Name, TableElementData.TableElementType,
      // TableElementData.Number, TableElementData.Value }), E); m_pEffect = null;
      // }
   }
}

void AssignedEffect::Init(Table* pTable) { ResolveEffectName(pTable); }

void AssignedEffect::Finish() { m_pEffect = nullptr; }

} // namespace DOF