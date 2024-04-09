#include "TableElement.h"

#include <iostream>
#include <limits>

#include "../fx/AssignedEffectList.h"
#include "TableElementData.h"

namespace DOF
{

TableElement::TableElement()
{
  m_pAssignedEffects = new AssignedEffectList();
  // ValueChanged += new EventHandler<TableElementValueChangedEventArgs>(TableElement_ValueChanged);

  m_number = (std::numeric_limits<int>::min)();
}

TableElement::TableElement(TableElementTypeEnum tableElementType, int number, int value) : TableElement()
{
  SetTableElementType(tableElementType);
  SetNumber(number);
  SetValue(value);
}

TableElement::TableElement(const std::string& szTableElementName, int value) : TableElement()
{
  SetTableElementType(TableElementTypeEnum::NamedElement);
  SetNumber((std::numeric_limits<int>::min)());
  SetName(szTableElementName);
  SetValue(value);
}

void TableElement::SetName(const std::string& szName)
{
  std::string szUpper = szName;
  std::transform(szUpper.begin(), szUpper.end(), szUpper.begin(), [](unsigned char c) { return std::toupper(c); });

  if (m_szName != szUpper)
  {
    m_szName = szUpper;
    // if (NameChanged) {
    // NameChanged(this, new EventArgs());
    //}
  }
}

void TableElement::SetValue(int value)
{
  if (m_value != value)
  {
    m_value = value;
    // if (ValueChanged) {
    // ValueChanged(this, new TableElementValueChangedEventArgs(m_value));
    //}
  }
}

TableElementData* TableElement::GetTableElementData() { return new TableElementData(this); }
}  // namespace DOF