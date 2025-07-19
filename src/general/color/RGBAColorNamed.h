#pragma once

#include "RGBAColor.h"
#include "RGBColor.h"
#include "../generic/IXmlSerializable.h"
#include <string>

namespace DOF
{

class RGBAColorNamed : public RGBAColor, public IXmlSerializable
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

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "RGBAColorNamed"; }
};

}