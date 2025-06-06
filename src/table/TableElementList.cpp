#include "TableElementList.h"

#include "../fx/AssignedEffectList.h"
#include "TableElement.h"
#include "TableElementData.h"

namespace DOF
{

void TableElementList::InitAssignedEffects(Table* pTable)
{
   for (TableElement* pTableElement : *this)
      pTableElement->GetAssignedEffects()->Init(pTable);
}

void TableElementList::FinishAssignedEffects()
{
   for (TableElement* pTableElement : *this)
      pTableElement->GetAssignedEffects()->Finish();
}

void TableElementList::UpdateState(TableElementData* pData) { }

} // namespace DOF