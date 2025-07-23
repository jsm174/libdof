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
   ~TableElementList();
   void Init(Table* table);
   void FinishAssignedEffects();
   void UpdateState(TableElementData* data);

private:
};

}
