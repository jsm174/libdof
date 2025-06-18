#include "EffectBase.h"
#include "../Log.h"

namespace DOF
{

EffectBase::EffectBase() { }

void EffectBase::Finish() { }

tinyxml2::XMLElement* EffectBase::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   if (!GetName().empty())
   {
      tinyxml2::XMLElement* nameElement = doc.NewElement("Name");
      nameElement->SetText(GetName().c_str());
      element->InsertEndChild(nameElement);
   }

   SerializeToXml(element, doc);

   return element;
}

bool EffectBase::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   const tinyxml2::XMLElement* nameElement = element->FirstChildElement("Name");
   if (nameElement && nameElement->GetText())
   {
      SetName(nameElement->GetText());
   }

   DeserializeFromXml(element);

   return true;
}

void EffectBase::SerializeToXml(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& doc) const { }

void EffectBase::DeserializeFromXml(const tinyxml2::XMLElement* element) { }

}