#pragma once

#include "DOF/DOF.h"
#include "out/IOutput.h"

namespace DOF
{

class Cabinet;

class CabinetOutputList : public std::vector<IOutput*>
{
 public:
  CabinetOutputList(Cabinet* pCabinet);

 private:
  Cabinet* m_pCabinet;
};

}  // namespace DOF
