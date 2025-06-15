#pragma once

#include "IOutputController.h"
#include <cstdint>

namespace DOF
{

class ISupportsSetValues : public virtual IOutputController
{
public:
   virtual void SetValues(int firstOutput, const uint8_t* values, int valueCount) = 0;
   virtual ~ISupportsSetValues() = default;
};

}