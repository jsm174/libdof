#pragma once

#include <string>

#include <tinyxml2/tinyxml2.h>

namespace DOF
{

class IXmlSerializable
{
public:
   virtual ~IXmlSerializable() = default;

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const = 0;
   virtual bool FromXml(const tinyxml2::XMLElement* element) = 0;
   virtual std::string GetXmlElementName() const = 0;
};

}