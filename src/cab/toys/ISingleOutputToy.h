#pragma once

#include "IToy.h"
#include <string>

namespace DOF
{

class ISingleOutputToy : public virtual IToy
{
public:
   virtual ~ISingleOutputToy() = default;

   virtual const std::string& GetOutputName() const = 0;
   virtual void SetOutputName(const std::string& name) = 0;
};

}