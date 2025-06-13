#include "ShapeList.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"

#include <tinyxml2/tinyxml2.h>

using namespace tinyxml2;

namespace DOF
{

ShapeList::ShapeList() { }

ShapeList::~ShapeList() { }

void ShapeList::WriteXml(std::string& writer)
{
   XMLDocument doc;
   XMLElement* root = doc.NewElement("ShapeList");
   doc.InsertFirstChild(root);

   for (auto* shape : *this)
   {
      XMLElement* shapeElement = doc.NewElement(typeid(*shape).name());
      root->InsertEndChild(shapeElement);
   }

   XMLPrinter printer;
   doc.Print(&printer);
   writer = std::string(printer.CStr());
}

void ShapeList::ReadXml(const std::string& reader)
{
   using namespace tinyxml2;

   XMLDocument doc;
   XMLError result = doc.Parse(reader.c_str());
   if (result != XML_SUCCESS)
      return;

   XMLElement* root = doc.FirstChildElement("ShapeList");
   if (!root)
      return;

   for (XMLElement* element = root->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
   {
      std::string typeName = element->Name();

      Log::Warning(StringExtensions::Build("ShapeDefinition type {0} not found during deserialization of cabinet data.", typeName));
   }
}

std::string ShapeList::GetSchema() { return ""; }

}