#include "RGBAColorNamed.h"
#include "RGBColor.h"

namespace DOF
{

RGBAColorNamed::RGBAColorNamed()
   : RGBAColor()
   , m_name("")
{
}

RGBAColorNamed::RGBAColorNamed(const std::string& name, int brightnessRed, int brightnessGreen, int brightnessBlue)
   : RGBAColor(brightnessRed, brightnessGreen, brightnessBlue)
   , m_name(name)
{
}

RGBAColorNamed::RGBAColorNamed(const std::string& name, int brightnessRed, int brightnessGreen, int brightnessBlue, int alpha)
   : RGBAColor(brightnessRed, brightnessGreen, brightnessBlue, alpha)
   , m_name(name)
{
}

RGBAColorNamed::RGBAColorNamed(const std::string& name, const std::string& color)
   : RGBAColor(color)
   , m_name(name)
{
}

RGBAColorNamed::RGBAColorNamed(const std::string& name, const RGBColor& rgbColor)
   : RGBAColor(rgbColor)
   , m_name(name)
{
}

}