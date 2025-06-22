#include "EffectList.h"
#include "IEffect.h"
#include "EffectFactory.h"
#include "../Log.h"
#include "../general/StringExtensions.h"

namespace DOF
{

void EffectList::Init(Table* table)
{
   for (auto& pair : *this)
   {
      if (pair.second != nullptr)
      {
         pair.second->Init(table);
      }
   }
}

void EffectList::Finish()
{
   for (auto& pair : *this)
   {
      if (pair.second != nullptr)
      {
         pair.second->Finish();
         delete pair.second;
      }
   }
   clear();
}

tinyxml2::XMLElement* EffectList::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* effectsElement = doc.NewElement(GetXmlElementName().c_str());

   for (const auto& pair : *this)
   {
      if (pair.second != nullptr)
      {
         tinyxml2::XMLElement* effectElement = pair.second->ToXml(doc);
         if (effectElement)
         {
            effectsElement->InsertEndChild(effectElement);
         }
      }
   }

   return effectsElement;
}

bool EffectList::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   Finish();

   const EffectFactory& factory = EffectFactory::GetInstance();

   const tinyxml2::XMLElement* effectElement = element->FirstChildElement();
   while (effectElement)
   {
      std::string typeName = effectElement->Name();

      IEffect* effect = factory.CreateEffect(typeName);
      if (effect)
      {

         if (effect->FromXml(effectElement))
         {
            Add(effect);
         }
         else
         {
            Log::Warning(StringExtensions::Build("Failed to deserialize effect of type: {0}", typeName));
            delete effect;
         }
      }
      else
      {
         Log::Warning(StringExtensions::Build("Unknown effect type: {0}", typeName));
      }

      effectElement = effectElement->NextSiblingElement();
   }

   return true;
}

void EffectList::Add(IEffect* effect)
{
   if (effect && !effect->GetName().empty())
   {
      (*this)[effect->GetName()] = effect;
   }
}

bool EffectList::Contains(const std::string& name) const { return find(name) != end(); }

}