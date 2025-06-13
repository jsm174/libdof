#include "EffectBase.h"
#include "../Log.h"

namespace DOF
{

EffectBase::EffectBase() { }

void EffectBase::Finish() { }

XMLElement* EffectBase::ToXml(XMLDocument& doc) const
{
   XMLElement* element = doc.NewElement(GetXmlElementName().c_str());


   if (!GetName().empty())
   {
      XMLElement* nameElement = doc.NewElement("Name");
      nameElement->SetText(GetName().c_str());
      element->InsertEndChild(nameElement);
   }


   SerializeToXml(element, doc);

   return element;
}

bool EffectBase::FromXml(const XMLElement* element)
{
   if (!element)
      return false;


   const XMLElement* nameElement = element->FirstChildElement("Name");
   if (nameElement && nameElement->GetText())
   {
      SetName(nameElement->GetText());
   }


   DeserializeFromXml(element);

   return true;
}

void EffectBase::SerializeToXml(XMLElement* element, XMLDocument& doc) const { }

void EffectBase::DeserializeFromXml(const XMLElement* element) { }

}