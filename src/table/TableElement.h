
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
   ~TableElement();


   TableElement(const TableElement&) = delete;
   TableElement& operator=(const TableElement&) = delete;

   void SetTableElementType(TableElementTypeEnum tableElementType) { m_tableElementType = tableElementType; }
   TableElementTypeEnum GetTableElementType() const { return m_tableElementType; }
   int GetNumber() const { return m_number; }
   void SetNumber(int number) { m_number = number; }
   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name);

   int GetValue() const { return m_value; }
   void SetValue(int value);


   AssignedEffectList* GetAssignedEffects() { return m_assignedEffects; }
   void SetAssignedEffects(AssignedEffectList* assignedEffects) { m_assignedEffects = assignedEffects; }
   TableElementData GetTableElementData();


   std::string ToXml() const;
   static TableElement* FromXml(const std::string& xml);

private:
   TableElementTypeEnum m_tableElementType;
   int m_number;
   std::string m_name;
   int m_value;
   AssignedEffectList* m_assignedEffects;
};

}