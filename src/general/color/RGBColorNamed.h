#pragma once

#include "RGBColor.h"
#include <string>

namespace DOF
{

class RGBColorNamed : public RGBColor
{
private:
   std::string m_name;

public:
   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& value) { m_name = value; }
   RGBColorNamed();
   RGBColorNamed(const std::string& name, int brightnessRed, int brightnessGreen, int brightnessBlue);
   RGBColorNamed(const std::string& name, const std::string& color);
};

}