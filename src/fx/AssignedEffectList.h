#pragma once

#include "AssignedEffect.h"
#include "DOF/DOF.h"

namespace DOF
{

class Table;
class TableElementData;

class AssignedEffectList : public std::vector<AssignedEffect*>
{
 public:
  void Add(const std::string& szEffectName);
  void Trigger(TableElementData* pTableElementData);
  void Init(Table* pTable);
  void Finish();
};

}  // namespace DOF
