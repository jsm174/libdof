#pragma once

#include "../Cabinet.h"

namespace DOF
{

class IAutoConfigOutputController
{
 public:
  IAutoConfigOutputController() {}
  ~IAutoConfigOutputController() {}

  virtual void AutoConfig(Cabinet* pCabinet) = 0;
};

}  // namespace DOF
