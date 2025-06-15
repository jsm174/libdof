#include "ShapeDefinitions.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include "../../../general/FileReader.h"

#include <tinyxml2/tinyxml2.h>

#include <fstream>
#include <sstream>

using namespace tinyxml2;

namespace DOF
{

ShapeDefinitions::ShapeDefinitions()
   : m_bitmapFilePattern(nullptr)
{
}

ShapeDefinitions::~ShapeDefinitions() { delete m_bitmapFilePattern; }

FilePattern* ShapeDefinitions::GetBitmapFilePattern() const { return m_bitmapFilePattern; }

void ShapeDefinitions::SetBitmapFilePattern(FilePattern* value)
{
   delete m_bitmapFilePattern;
   m_bitmapFilePattern = value;
}

ShapeList& ShapeDefinitions::GetShapes() { return m_shapes; }

void ShapeDefinitions::SetShapes(const ShapeList& value) { m_shapes = value; }

std::string ShapeDefinitions::GetShapeDefinitionsXml()
{
   XMLDocument doc;
   XMLElement* root = doc.NewElement("ShapeDefinitions");
   doc.InsertFirstChild(root);

   XMLElement* shapesElement = doc.NewElement("Shapes");
   std::string shapesXml;
   m_shapes.WriteXml(shapesXml);

   XMLDocument shapesDoc;
   if (shapesDoc.Parse(shapesXml.c_str()) == XML_SUCCESS)
   {
      XMLElement* shapesRoot = shapesDoc.FirstChildElement();
      if (shapesRoot)
      {
         XMLElement* copiedElement = shapesRoot->DeepClone(&doc)->ToElement();
         root->InsertEndChild(copiedElement);
      }
   }

   XMLPrinter printer;
   doc.Print(&printer);
   return std::string(printer.CStr());
}

void ShapeDefinitions::SaveShapeDefinitionsXmlFile(const std::string& fileName)
{
   std::string xml = GetShapeDefinitionsXml();
   std::ofstream file(fileName);
   if (file.is_open())
   {
      file << xml;
      file.close();
   }
}

ShapeDefinitions* ShapeDefinitions::GetShapeDefinitionsFromShapeDefinitionsXmlFile(const std::string& fileName)
{
   std::string xml;
   try
   {
      xml = FileReader::ReadFileToString(fileName);
   }
   catch (const std::exception& e)
   {
      Log::Exception(StringExtensions::Build("Could not load ShapeDefinitions from {0}.", fileName));
      throw std::runtime_error(StringExtensions::Build("Could not read ShapeDefinitions file {0}.", fileName));
   }

   return GetShapeDefinitionsFromShapeDefinitionsXml(xml);
}

bool ShapeDefinitions::TestShapeDefinitionsShapeDefinitionsXmlFile(const std::string& fileName)
{
   ShapeDefinitions* c = nullptr;
   try
   {
      c = GetShapeDefinitionsFromShapeDefinitionsXmlFile(fileName);
   }
   catch (...)
   {
      return false;
   }
   bool result = c != nullptr;
   delete c;
   return result;
}

ShapeDefinitions* ShapeDefinitions::GetShapeDefinitionsFromShapeDefinitionsXml(const std::string& shapeDefinitionsXml)
{
   using namespace tinyxml2;

   XMLDocument doc;
   XMLError result = doc.Parse(shapeDefinitionsXml.c_str());
   if (result != XML_SUCCESS)
   {
      Log::Exception("Could not deserialize the ShapeDefinitions from XML data.");
      throw std::runtime_error("Could not deserialize the ShapeDefinitions from XML data.");
   }

   XMLElement* root = doc.FirstChildElement("ShapeDefinitions");
   if (!root)
   {
      throw std::runtime_error("Could not find ShapeDefinitions root element.");
   }

   ShapeDefinitions* shapeDefinitions = new ShapeDefinitions();

   XMLElement* shapesElement = root->FirstChildElement("Shapes");
   if (shapesElement)
   {
      XMLPrinter printer;
      shapesElement->Accept(&printer);
      std::string shapesXml = printer.CStr();
      shapeDefinitions->m_shapes.ReadXml(shapesXml);
   }

   return shapeDefinitions;
}

}