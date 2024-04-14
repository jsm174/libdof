#pragma once

#include "DOF/DOF.h"

namespace DOF
{

class Table;
class TableElement;
class TableElementData;

class TableElementList : public std::vector<TableElement*>
{
 public:
  void InitAssignedEffects(Table* pTable);
  void FinishAssignedEffects();
  void UpdateState(TableElementData* pData);

 private:
  // Dictionary<TableElementTypeEnum, Dictionary<int, TableElement>> _NumberedTableElementsDictionary;
  // Dictionary<string, TableElement> _NamedTableElementsDictionary;
};

}  // namespace DOF
