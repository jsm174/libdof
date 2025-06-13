#pragma once

#include "RGBAColor.h"
#include "RGBColor.h"
#include <string>

namespace DOF
{

class RGBAColorNamed : public RGBAColor
{
private:
   std::string m_name;

public:
   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& value) { m_name = value; }
   RGBAColorNamed();
   RGBAColorNamed(const std::string& name, int brightnessRed, int brightnessGreen, int brightnessBlue);
   RGBAColorNamed(const std::string& name, int brightnessRed, int brightnessGreen, int brightnessBlue, int alpha);
   RGBAColorNamed(const std::string& name, const std::string& color);
   RGBAColorNamed(const std::string& name, const RGBColor& rgbColor);
};

}