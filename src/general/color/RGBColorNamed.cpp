#include "RGBColorNamed.h"

namespace DOF
{

RGBColorNamed::RGBColorNamed()
   : RGBColor()
   , m_name("")
{
}

RGBColorNamed::RGBColorNamed(const std::string& name, int brightnessRed, int brightnessGreen, int brightnessBlue)
   : RGBColor(brightnessRed, brightnessGreen, brightnessBlue)
   , m_name(name)
{
}

RGBColorNamed::RGBColorNamed(const std::string& name, const std::string& color)
   : RGBColor(color)
   , m_name(name)
{
}

}