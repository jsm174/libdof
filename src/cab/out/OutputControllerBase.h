#pragma once

#include "DOF/DOF.h"
#include "IOutputController.h"

namespace DOF
{

class OutputControllerBase : public IOutputController
{
public:
   OutputControllerBase();
   ~OutputControllerBase();
};

} // namespace DOF
