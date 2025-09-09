#include "ShapeDefinitions.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include "../../../general/FileReader.h"

#include <tinyxml2/tinyxml2.h>

#include <fstream>
#include <sstream>

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
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* root = doc.NewElement("ShapeDefinitions");
   doc.InsertFirstChild(root);

   tinyxml2::XMLElement* shapesElement = doc.NewElement("Shapes");
   std::string shapesXml;
   m_shapes.WriteXml(shapesXml);

   tinyxml2::XMLDocument shapesDoc;
   if (shapesDoc.Parse(shapesXml.c_str()) == tinyxml2::XML_SUCCESS)
   {
      tinyxml2::XMLElement* shapesRoot = shapesDoc.FirstChildElement();
      if (shapesRoot)
      {
         tinyxml2::XMLElement* copiedElement = shapesRoot->DeepClone(&doc)->ToElement();
         root->InsertEndChild(copiedElement);
      }
   }

   tinyxml2::XMLPrinter printer;
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
      if (xml.empty())
      {
         Log::Warning(StringExtensions::Build("ShapeDefinitions file {0} is empty.", fileName));
         return nullptr;
      }
   }
   catch (const std::exception& e)
   {
      Log::Exception(StringExtensions::Build("Could not load ShapeDefinitions from {0}. Error: {1}", fileName, e.what()));
      throw std::runtime_error(StringExtensions::Build("Could not read ShapeDefinitions file {0}.", fileName));
   }

   try
   {
      return GetShapeDefinitionsFromShapeDefinitionsXml(xml);
   }
   catch (const std::exception& e)
   {
      Log::Exception(StringExtensions::Build("Could not parse ShapeDefinitions XML from {0}. Error: {1}", fileName, e.what()));
      return nullptr;
   }
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
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLError result = doc.Parse(shapeDefinitionsXml.c_str());
   if (result != tinyxml2::XML_SUCCESS)
   {
      Log::Exception("Could not load ShapeDefinitions from XML data.");
      throw std::runtime_error("Could not deserialize the ShapeDefinitions from XML data.");
   }

   tinyxml2::XMLElement* root = doc.FirstChildElement("ShapeDefinitions");
   if (!root)
   {
      Log::Exception("Could not find ShapeDefinitions root element in XML");
      throw std::runtime_error("Could not find ShapeDefinitions root element.");
   }

   ShapeDefinitions* shapeDefinitions = new ShapeDefinitions();

   tinyxml2::XMLElement* shapesElement = root->FirstChildElement("Shapes");
   if (shapesElement)
   {
      try
      {
         for (tinyxml2::XMLElement* shapeElement = shapesElement->FirstChildElement(); shapeElement != nullptr; shapeElement = shapeElement->NextSiblingElement())
         {
            std::string elementName = shapeElement->Name() ? shapeElement->Name() : "";

            if (elementName == "Shape")
            {
               Shape* shape = new Shape();
               bool shapeValid = true;

               try
               {
                  tinyxml2::XMLElement* nameElement = shapeElement->FirstChildElement("Name");
                  if (nameElement && nameElement->GetText())
                     shape->SetName(nameElement->GetText());

                  tinyxml2::XMLElement* frameElement = shapeElement->FirstChildElement("BitmapFrameNumber");
                  if (frameElement)
                     shape->SetBitmapFrameNumber(frameElement->IntText(0));

                  tinyxml2::XMLElement* topElement = shapeElement->FirstChildElement("BitmapTop");
                  if (topElement)
                     shape->SetBitmapTop(topElement->IntText(0));

                  tinyxml2::XMLElement* leftElement = shapeElement->FirstChildElement("BitmapLeft");
                  if (leftElement)
                     shape->SetBitmapLeft(leftElement->IntText(0));

                  tinyxml2::XMLElement* widthElement = shapeElement->FirstChildElement("BitmapWidth");
                  if (widthElement)
                     shape->SetBitmapWidth(widthElement->IntText(0));

                  tinyxml2::XMLElement* heightElement = shapeElement->FirstChildElement("BitmapHeight");
                  if (heightElement)
                     shape->SetBitmapHeight(heightElement->IntText(0));

                  if (!shape->GetName().empty() && !shapeDefinitions->m_shapes.Contains(shape->GetName()))
                  {
                     shapeDefinitions->m_shapes.Add(shape);
                  }
                  else
                  {
                     shapeValid = false;
                  }
               }
               catch (const std::exception& e)
               {
                  Log::Warning(StringExtensions::Build("Error processing shape: {0}", e.what()));
                  shapeValid = false;
               }

               if (!shapeValid)
               {
                  delete shape;
               }
            }
         }
      }
      catch (const std::exception& e)
      {
         Log::Exception(StringExtensions::Build("Exception during shape processing: {0}", e.what()));
         return shapeDefinitions;
      }
   }

   return shapeDefinitions;
}

}