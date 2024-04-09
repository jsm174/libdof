#pragma once

#include "DOF/DOF.h"

namespace DOF
{

class GlobalConfiguration
{
 public:
  static GlobalConfiguration* GetInstance();

  FILE* GetCabinetConfigFile();

 private:
  GlobalConfiguration(){};
  ~GlobalConfiguration(){};

  static GlobalConfiguration* m_pInstance;
};

}  // namespace DOF
