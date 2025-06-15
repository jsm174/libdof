#pragma once

#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace tinyxml2;

namespace DOF
{

class IXmlSerializable
{
public:
   virtual ~IXmlSerializable() = default;

   virtual XMLElement* ToXml(XMLDocument& doc) const = 0;
   virtual bool FromXml(const XMLElement* element) = 0;
   virtual std::string GetXmlElementName() const = 0;
};

}