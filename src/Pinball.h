#pragma once

#include "DOF/DOF.h"

#ifdef __HIDAPI__
#include "cab/out/ps/Pinscape.h"
#endif

namespace DOF
{

class Pinball
{
 public:
  Pinball();
  ~Pinball();

  void ReceiveData(char type, int number, int value);

 private:
#ifdef __HIDAPI__
  Pinscape* m_pPinscape;
#endif
};

}  // namespace DOF
