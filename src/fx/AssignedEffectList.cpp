#include "AssignedEffectList.h"
#include "../table/Table.h"
#include "../table/TableElementData.h"

namespace DOF
{

void AssignedEffectList::Add(const std::string& effectName)
{
   if (!effectName.empty())
   {
      AssignedEffect* assignedEffect = new AssignedEffect(effectName);
      push_back(assignedEffect);
   }
}

void AssignedEffectList::Trigger(TableElementData* pTableElementData)
{
   for (AssignedEffect* pAssignedEffect : *this)
   {
      if (pAssignedEffect != nullptr)
         pAssignedEffect->Trigger(pTableElementData);
   }
}

void AssignedEffectList::Init(Table* pTable)
{
   for (AssignedEffect* pAssignedEffect : *this)
   {
      if (pAssignedEffect != nullptr)
         pAssignedEffect->Init(pTable);
   }
}

void AssignedEffectList::Finish()
{
   for (AssignedEffect* pAssignedEffect : *this)
   {
      if (pAssignedEffect != nullptr)
      {
         pAssignedEffect->Finish();
         delete pAssignedEffect;
      }
   }
   clear();
}

}