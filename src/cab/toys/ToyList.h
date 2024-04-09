#pragma once

#include "DOF/DOF.h"
#include "IToy.h"

namespace DOF
{

class ToyList : public std::vector<IToy*>
{
 public:
  void WriteXml();
  void ReadXml();
  void Init(Cabinet* pCabinet);
  void Reset();
  void Finish();
  void UpdateOutputs();
};

}  // namespace DOF
