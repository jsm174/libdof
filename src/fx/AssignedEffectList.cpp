#include "AssignedEffectList.h"

#include "../table/Table.h"
#include "../table/TableElementData.h"

namespace DOF
{

void AssignedEffectList::Add(const std::string& effectName) { }

void AssignedEffectList::Trigger(TableElementData* pTableElementData)
{
   for (AssignedEffect* pAssignedEffect : *this)
      pAssignedEffect->Trigger(pTableElementData);
}

void AssignedEffectList::Init(Table* pTable)
{
   for (AssignedEffect* pAssignedEffect : *this)
      pAssignedEffect->Init(pTable);
}

void AssignedEffectList::Finish()
{
   for (AssignedEffect* pAssignedEffect : *this)
      pAssignedEffect->Finish();
}

} // namespace DOF
// namespace DOF