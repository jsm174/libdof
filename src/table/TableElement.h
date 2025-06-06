
#pragma once

#include "DOF/DOF.h"
#include "TableElementTypeEnum.h"

namespace DOF
{

class Table;
class TableElementData;
class AssignedEffectList;

class TableElement
{
public:
   TableElement();
   TableElement(TableElementTypeEnum tableElementType, int number, int value);
   TableElement(const std::string& tableElementName, int value);
   ~TableElement() { }

   void SetTableElementType(TableElementTypeEnum tableElementType) { m_tableElementType = tableElementType; }
   TableElementTypeEnum GetTableElementType() const { return m_tableElementType; }
   int GetNumber() const { return m_number; }
   void SetNumber(int number) { m_number = number; }
   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name);
   // event EventHandler<EventArgs> NameChanged;
   int GetValue() const { return m_value; }
   void SetValue(int value);
   // event EventHandler<TableElementValueChangedEventArgs> ValueChanged;
   // void TableElement_ValueChanged(object sender, TableElementValueChangedEventArgs e)
   //  get AssignedEffectList
   AssignedEffectList* GetAssignedEffects() { return m_pAssignedEffects; }
   void SetAssignedEffects(AssignedEffectList* pAssignedEffects) { m_pAssignedEffects = pAssignedEffects; }
   TableElementData* GetTableElementData();

private:
   TableElementTypeEnum m_tableElementType;
   int m_number;
   std::string m_name;
   int m_value;
   AssignedEffectList* m_pAssignedEffects;
};

} // namespace DOF