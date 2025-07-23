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

void AssignedEffectList::Trigger(TableElementData tableElementData)
{
   for (AssignedEffect* assignedEffect : *this)
   {
      if (assignedEffect != nullptr)
         assignedEffect->Trigger(tableElementData);
   }
}

void AssignedEffectList::Init(Table* table)
{
   for (AssignedEffect* assignedEffect : *this)
   {
      if (assignedEffect != nullptr)
         assignedEffect->Init(table);
   }
}

void AssignedEffectList::Finish()
{
   for (AssignedEffect* assignedEffect : *this)
   {
      if (assignedEffect != nullptr)
      {
         assignedEffect->Finish();
         delete assignedEffect;
      }
   }
   clear();
}

}