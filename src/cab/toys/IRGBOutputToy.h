#pragma once

#include "IToy.h"
#include <string>

namespace DOF
{

class IRGBOutputToy : public virtual IToy
{
public:
   virtual ~IRGBOutputToy() = default;

   virtual const std::string& GetOutputNameBlue() const = 0;
   virtual void SetOutputNameBlue(const std::string& name) = 0;
   virtual const std::string& GetOutputNameGreen() const = 0;
   virtual void SetOutputNameGreen(const std::string& name) = 0;
   virtual const std::string& GetOutputNameRed() const = 0;
   virtual void SetOutputNameRed(const std::string& name) = 0;
};

}