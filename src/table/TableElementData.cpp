#include "TableElementData.h"

#include <limits>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <cstdlib>

#include "TableElement.h"
#include "../Log.h"
#include "../general/StringExtensions.h"

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
   if (!IsValidTableElementTypeChar(tableElementTypeChar))
   {
      Log::Warning(StringExtensions::Build("Undefined char \"{0}\" supplied for the TableElementTypeChar.", std::string(1, tableElementTypeChar)));
      m_tableElementType = TableElementTypeEnum::Unknown;
   }
   else
   {
      m_tableElementType = (TableElementTypeEnum)tableElementTypeChar;
   }
   m_number = number;
   m_name = "";
   m_value = value;
}

TableElementData::TableElementData(TableElement* tableElement)
{
   m_number = tableElement->GetNumber();
   m_tableElementType = tableElement->GetTableElementType();
   m_name = tableElement->GetName();
   m_value = tableElement->GetValue();
}

TableElementData::TableElementData(const std::string& tableElementName, int value)
{
   if (tableElementName.length() > 1 && IsValidTableElementTypeChar(tableElementName[0]) && tableElementName[0] != (char)TableElementTypeEnum::NamedElement
      && StringExtensions::IsInteger(tableElementName.substr(1)))
   {
      m_tableElementType = (TableElementTypeEnum)tableElementName[0];
      m_number = StringExtensions::ToInteger(tableElementName.substr(1));
      m_name = "";
   }
   else
   {
      m_tableElementType = TableElementTypeEnum::NamedElement;
      m_name = (tableElementName[0] != (char)TableElementTypeEnum::NamedElement) ? tableElementName : tableElementName.substr(1);
      m_number = (std::numeric_limits<int>::min)();
   }
   m_value = value;
}

int TableElementData::stringToInt(const std::string& s) { return StringExtensions::ToInteger(s); }

bool TableElementData::IsInteger(const std::string& s) { return StringExtensions::IsInteger(s); }

bool TableElementData::IsValidTableElementTypeChar(char c)
{
   return c == (char)TableElementTypeEnum::Unknown || c == (char)TableElementTypeEnum::Lamp || c == (char)TableElementTypeEnum::Switch || c == (char)TableElementTypeEnum::Solenoid
      || c == (char)TableElementTypeEnum::GIString || c == (char)TableElementTypeEnum::Mech || c == (char)TableElementTypeEnum::GetMech || c == (char)TableElementTypeEnum::EMTable
      || c == (char)TableElementTypeEnum::LED || c == (char)TableElementTypeEnum::Score || c == (char)TableElementTypeEnum::ScoreDigit || c == (char)TableElementTypeEnum::NamedElement;
}

}