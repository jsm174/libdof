#include "ShapeList.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"

#include <tinyxml2/tinyxml2.h>

namespace DOF
{

ShapeList::ShapeList() { }

ShapeList::~ShapeList() { }

void ShapeList::WriteXml(std::string& writer)
{
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* root = doc.NewElement("ShapeList");
   doc.InsertFirstChild(root);

   for (auto* shape : *this)
   {
      tinyxml2::XMLElement* shapeElement = doc.NewElement(typeid(*shape).name());
      root->InsertEndChild(shapeElement);
   }

   tinyxml2::XMLPrinter printer;
   doc.Print(&printer);
   writer = std::string(printer.CStr());
}

void ShapeList::ReadXml(const std::string& reader)
{
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLError result = doc.Parse(reader.c_str());
   if (result != tinyxml2::XML_SUCCESS)
      return;

   tinyxml2::XMLElement* root = doc.FirstChildElement("ShapeList");
   if (!root)
      return;

   for (tinyxml2::XMLElement* element = root->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
   {
      std::string typeName = element->Name();

      Log::Warning(StringExtensions::Build("ShapeDefinition type {0} not found during deserialization of cabinet data.", typeName));
   }
}

std::string ShapeList::GetSchema() { return ""; }

}