#include "TableElement.h"

#include <algorithm>
#include <iostream>
#include <limits>

#include "../fx/AssignedEffectList.h"
#include "TableElementData.h"
#include "../Log.h"
#include "../general/StringExtensions.h"

namespace DOF
{

TableElement::TableElement()
{
   m_tableElementType = TableElementTypeEnum::Lamp;
   m_number = (std::numeric_limits<int>::min)();
   m_name = "";
   m_value = (std::numeric_limits<int>::min)();
   m_pAssignedEffects = new AssignedEffectList();
}

TableElement::TableElement(TableElementTypeEnum tableElementType, int number, int value)
   : TableElement()
{
   SetTableElementType(tableElementType);
   SetNumber(number);
   SetValue(value);
}

TableElement::TableElement(const std::string& tableElementName, int value)
   : TableElement()
{
   SetTableElementType(TableElementTypeEnum::NamedElement);
   SetNumber((std::numeric_limits<int>::min)());
   SetName(tableElementName);
   SetValue(value);
}

void TableElement::SetName(const std::string& name)
{
   std::string upper = StringExtensions::ToUpper(name);

   if (m_name != upper)
   {
      m_name = upper;
   }
}

void TableElement::SetValue(int value)
{
   if (m_value != value)
   {
      m_value = value;

      if (m_pAssignedEffects)
      {
         TableElementData* pData = GetTableElementData();
         m_pAssignedEffects->Trigger(pData);
         delete pData;
      }
   }
}

TableElement::~TableElement()
{
   delete m_pAssignedEffects;
   m_pAssignedEffects = nullptr;
}

TableElementData* TableElement::GetTableElementData() { return new TableElementData(this); }

std::string TableElement::ToXml() const
{
   tinyxml2::XMLDocument doc;
   doc.InsertEndChild(doc.NewDeclaration());

   tinyxml2::XMLElement* root = doc.NewElement("TableElement");
   doc.InsertEndChild(root);

   tinyxml2::XMLElement* element = doc.NewElement("TableElementType");
   element->SetText((int)m_tableElementType);
   root->InsertEndChild(element);

   element = doc.NewElement("Number");
   element->SetText(m_number);
   root->InsertEndChild(element);

   if (!m_name.empty())
   {
      element = doc.NewElement("Name");
      element->SetText(m_name.c_str());
      root->InsertEndChild(element);
   }

   if (m_pAssignedEffects)
   {
      tinyxml2::XMLElement* effectsElement = doc.NewElement("AssignedEffects");

      root->InsertEndChild(effectsElement);
   }

   tinyxml2::XMLPrinter printer;
   doc.Print(&printer);
   return std::string(printer.CStr());
}

TableElement* TableElement::FromXml(const std::string& xml)
{
   tinyxml2::XMLDocument doc;
   if (doc.Parse(xml.c_str()) != tinyxml2::XML_SUCCESS)
   {
      Log::Warning(StringExtensions::Build("TableElement XML parse error: {0}", doc.ErrorStr()));
      return nullptr;
   }

   tinyxml2::XMLElement* root = doc.FirstChildElement("TableElement");
   if (!root)
   {
      Log::Warning("TableElement root element not found");
      return nullptr;
   }

   TableElement* pElement = new TableElement();

   tinyxml2::XMLElement* element = root->FirstChildElement("TableElementType");
   if (element)
   {
      int value;
      if (element->QueryIntText(&value) == tinyxml2::XML_SUCCESS)
         pElement->SetTableElementType((TableElementTypeEnum)value);
   }

   element = root->FirstChildElement("Number");
   if (element)
   {
      int value;
      if (element->QueryIntText(&value) == tinyxml2::XML_SUCCESS)
         pElement->SetNumber(value);
   }

   element = root->FirstChildElement("Name");
   if (element && element->GetText())
      pElement->SetName(element->GetText());

   element = root->FirstChildElement("AssignedEffects");
   if (element)
   {
   }

   return pElement;
}

}