#pragma once

#include <map>

#include "DOF/DOF.h"

namespace DOF
{

class IEffect;
class Table;

class EffectList : public std::map<std::string, IEffect*>
{
 public:
  void Finish();
  void Init(Table* pTable);
};

}  // namespace DOF
