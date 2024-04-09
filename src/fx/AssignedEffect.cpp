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

AssignedEffect::AssignedEffect(const std::string& szEffectName)
{
  m_pEffect = nullptr;

  // EffectNameChanged += new EventHandler<EventArgs>(TableElementEffect_EffectNameChanged);

  SetEffectName(szEffectName);
}

void AssignedEffect::SetEffectName(const std::string& szEffectName)
{
  if (m_szEffectName != szEffectName)
  {
    m_szEffectName = szEffectName;

    if (!m_szEffectName.empty())
    {
      // EffectNameChanged(this, new EventArgs());
    }
  }
}

void AssignedEffect::ResolveEffectName(Table* pTable)
{
  if (!m_szEffectName.empty())  // && pTable->GetEffects().ContainsKey(m_szEffectName))
  {
    // m_pEffect = pTable->GetEffects()[m_szEffectName];
  }
}

void AssignedEffect::Trigger(TableElementData* pTableElementData)
{
  if (m_pEffect)
  {
    // try {
    // m_pEffect->Trigger(pTableElementData);
    // } catch (Exception ex) {
    // Log("A exception occured when triggering effect {0} for table element {1} {2} with value {3}. Effect assignement
    // will be deactivated.".Build(new object[] { Effect.Name, TableElementData.TableElementType,
    // TableElementData.Number, TableElementData.Value }), E); m_pEffect = null;
    // }
  }
}

void AssignedEffect::Init(Table* pTable) { ResolveEffectName(pTable); }

void AssignedEffect::Finish() { m_pEffect = nullptr; }

}  // namespace DOF