#include "TableElementData.h"

#include <limits>
#include <stdexcept>

#include "TableElement.h"

namespace DOF
{

TableElementData::TableElementData(TableElementTypeEnum tableElementType, int number, int value)
{
   m_tableElementType = tableElementType;
   m_number = number;
   m_value = value;
   m_name = "";
}

TableElementData::TableElementData(char tableElementTypeChar, int number, int value)
{
   m_tableElementType = (TableElementTypeEnum)tableElementTypeChar;
   m_number = number;
   m_name = "";
   m_value = value;
}

TableElementData::TableElementData(TableElement* pTableElement)
{
   m_number = pTableElement->GetNumber();
   m_tableElementType = pTableElement->GetTableElementType();
   m_name = pTableElement->GetName();
   m_value = pTableElement->GetValue();
}

TableElementData::TableElementData(const std::string& tableElementName, int value)
{
   if (!tableElementName.empty() && /* Enum check here */ true)
   {
      // m_tableElementType = static_cast<TableElementTypeEnum>(tableElementName[0]);
      // m_number = stringToInt(tableElementName.substr(1));
      // m_name = "";
   }
   else
   {
      // m_tableElementType = TableElementTypeEnum::NamedElement;
      // m_name = (tableElementName[0] != static_cast<char>(TableElementTypeEnum::NamedElement)) ? tableElementName
      m_number = (std::numeric_limits<int>::min)();
   }
   m_value = value;
}

int TableElementData::stringToInt(const std::string& s)
{
   try
   {
      size_t pos = 0;
      int value = std::stoi(s, &pos);
      if (pos < s.length())
      {
      }
      return value;
   }
   catch (const std::invalid_argument& ia)
   {
      return 0;
   }
   catch (const std::out_of_range& oor)
   {
      return 0;
   }
}

} // namespace DOF