
#pragma once

#include "DOF/DOF.h"
#include "TableElementTypeEnum.h"

namespace DOF
{
class TableElement;

struct TableElementData
{
   TableElementData()
      : m_tableElementType(TableElementTypeEnum::Unknown)
      , m_number(0)
      , m_value(0)
      , m_name("")
   {
   }
   TableElementData(TableElementTypeEnum tableElementType, int number, int value);
   TableElementData(char tableElementTypeChar, int number, int value);
   TableElementData(TableElement* tableElement);
   TableElementData(const std::string& tableElementName, int value);

   TableElementTypeEnum m_tableElementType;
   int m_number;
   int m_value;
   std::string m_name;

private:
   int stringToInt(const std::string& s);
   bool IsInteger(const std::string& s);
   bool IsValidTableElementTypeChar(char c);
};

}