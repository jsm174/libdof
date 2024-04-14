
#pragma once

#include "DOF/DOF.h"
#include "TableElementTypeEnum.h"

namespace DOF
{
class TableElement;

class TableElementData
{
 public:
  TableElementData(TableElementTypeEnum tableElementType, int number, int value);
  TableElementData(char tableElementTypeChar, int number, int value);
  TableElementData(TableElement* pTableElement);
  TableElementData(const std::string& szTableElementName, int value);

  TableElementTypeEnum m_tableElementType;
  int m_number;
  int m_value;
  std::string m_szName;

 private:
  int stringToInt(const std::string& s);
};

}  // namespace DOF