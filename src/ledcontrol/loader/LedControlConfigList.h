#pragma once

#include <vector>

#include "DOF/DOF.h"

namespace DOF
{

class LedControlConfig;

class LedControlConfigList : public std::vector<LedControlConfig*>
{
public:
};

} // namespace DOF
