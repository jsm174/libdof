#include "DOF/DOF.h"

#include <algorithm>
#include <chrono>
#include <cstring>

#include "DOF/Config.h"
#include "Logger.h"

namespace DOF
{

DOF::DOF() {}

DOF::~DOF() {}

void DOF::DataReceive(char type, int number, int value)
{
  Log("DOF::DataReceive: type=%c, number=%d, value=%d", type, number, value);
}

}  // namespace DOF
