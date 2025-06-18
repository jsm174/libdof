#include "TableElementList.h"

#include "../fx/AssignedEffectList.h"
#include "TableElement.h"
#include "TableElementData.h"
#include "../Log.h"
#include "../general/StringExtensions.h"

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

void TableElementList::UpdateState(TableElementData* pData)
{
   if (!pData)
      return;

   TableElement* targetElement = nullptr;

   if (!pData->m_name.empty())
   {
      for (TableElement* element : *this)
      {
         if (element && element->GetName() == pData->m_name)
         {
            targetElement = element;
            break;
         }
      }
   }

   if (!targetElement)
   {
      for (TableElement* element : *this)
      {
         if (element && element->GetTableElementType() == pData->m_tableElementType && element->GetNumber() == pData->m_number)
         {
            targetElement = element;
            Log::Debug(StringExtensions::Build("Found matching element: type={0}, number={1}", std::string(1, (char)element->GetTableElementType()), std::to_string(element->GetNumber())));
            break;
         }
      }
   }

   if (targetElement)
      targetElement->SetValue(pData->m_value);
   else
   {
      TableElement* newElement = new TableElement(pData->m_tableElementType, pData->m_number, pData->m_value);
      push_back(newElement);
   }
}

}