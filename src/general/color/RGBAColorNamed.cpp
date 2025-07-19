#include "RGBAColorNamed.h"
#include "RGBColor.h"
#include <tinyxml2/tinyxml2.h>

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

tinyxml2::XMLElement* RGBAColorNamed::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   if (!m_name.empty())
   {
      element->SetAttribute("Name", m_name.c_str());
   }

   element->SetAttribute("HexColor", GetHexColor().c_str());

   return element;
}

bool RGBAColorNamed::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   const char* nameAttr = element->Attribute("Name");
   if (nameAttr)
   {
      m_name = nameAttr;
   }

   const char* hexColorAttr = element->Attribute("HexColor");
   if (hexColorAttr)
   {
      SetHexColor(hexColorAttr);
   }

   return true;
}

}