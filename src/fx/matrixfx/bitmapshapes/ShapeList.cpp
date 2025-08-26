#include "ShapeList.h"
#include "Shape.h"
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

   for (Shape* t : *this)
   {
      tinyxml2::XMLElement* shapeElement = doc.NewElement(typeid(*t).name());
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

   tinyxml2::XMLElement* root = doc.FirstChildElement();
   if (!root)
      return;

   for (tinyxml2::XMLElement* element = root->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
   {
      std::string t = element->Name();

      if (t == "Shape")
      {
         Shape* shape = new Shape();

         tinyxml2::XMLElement* nameElement = element->FirstChildElement("Name");
         if (nameElement)
            shape->SetName(nameElement->GetText() ? nameElement->GetText() : "");

         tinyxml2::XMLElement* frameElement = element->FirstChildElement("BitmapFrameNumber");
         if (frameElement)
            shape->SetBitmapFrameNumber(frameElement->IntText(0));

         tinyxml2::XMLElement* topElement = element->FirstChildElement("BitmapTop");
         if (topElement)
            shape->SetBitmapTop(topElement->IntText(0));

         tinyxml2::XMLElement* leftElement = element->FirstChildElement("BitmapLeft");
         if (leftElement)
            shape->SetBitmapLeft(leftElement->IntText(0));

         tinyxml2::XMLElement* widthElement = element->FirstChildElement("BitmapWidth");
         if (widthElement)
            shape->SetBitmapWidth(widthElement->IntText(0));

         tinyxml2::XMLElement* heightElement = element->FirstChildElement("BitmapHeight");
         if (heightElement)
            shape->SetBitmapHeight(heightElement->IntText(0));

         if (!this->Contains(shape->GetName()))
         {
            this->Add(shape);
         }
      }
      else
      {
         Log::Warning(StringExtensions::Build("ShapeDefinition type {0} not found during deserialization of cabinet data.", t));
      }
   }
}

std::string ShapeList::GetSchema() { return ""; }

}