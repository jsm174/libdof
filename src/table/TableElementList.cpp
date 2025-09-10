#include "TableElementList.h"

#include "../fx/AssignedEffectList.h"
#include "TableElement.h"
#include "TableElementData.h"
#include "../Log.h"
#include "../general/StringExtensions.h"

namespace DOF
{

TableElementList::~TableElementList()
{
   for (TableElement* element : *this)
   {
      delete element;
   }
   clear();
}

void TableElementList::Init(Table* table)
{
   for (TableElement* tableElement : *this)
      tableElement->GetAssignedEffects()->Init(table);
}

void TableElementList::FinishAssignedEffects()
{
   for (TableElement* tableElement : *this)
      tableElement->GetAssignedEffects()->Finish();
}

void TableElementList::UpdateState(TableElementData* data)
{
   if (!data)
      return;

   TableElement* targetElement = nullptr;

   if (!data->m_name.empty())
   {
      for (TableElement* element : *this)
      {
         if (element && element->GetName() == data->m_name)
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
         if (element && element->GetTableElementType() == data->m_tableElementType && element->GetNumber() == data->m_number)
         {
            targetElement = element;
            break;
         }
      }
   }

   if (targetElement)
      targetElement->SetValue(data->m_value);
   else
   {
      TableElement* newElement = new TableElement(data->m_tableElementType, data->m_number, data->m_value);
      push_back(newElement);
   }
}

}