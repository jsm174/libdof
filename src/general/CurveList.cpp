#include "CurveList.h"
#include <tinyxml2/tinyxml2.h>

namespace DOF
{

CurveList::CurveList() { }

CurveList::~CurveList() { Clear(); }

bool CurveList::Contains(const std::string& name) const { return m_curves.find(name) != m_curves.end(); }

Curve* CurveList::GetByName(const std::string& name) const
{
   auto it = m_curves.find(name);
   return (it != m_curves.end()) ? it->second : nullptr;
}

void CurveList::Add(const std::string& name, Curve* curve)
{
   if (curve != nullptr)
   {
      if (Contains(name))
      {
         Remove(name);
      }
      m_curves[name] = curve;
   }
}

void CurveList::Remove(const std::string& name)
{
   auto it = m_curves.find(name);
   if (it != m_curves.end())
   {
      delete it->second;
      m_curves.erase(it);
   }
}

void CurveList::Clear()
{
   for (auto& pair : m_curves)
   {
      delete pair.second;
   }
   m_curves.clear();
}

tinyxml2::XMLElement* CurveList::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   for (const auto& pair : m_curves)
   {
      if (pair.second != nullptr)
      {
         tinyxml2::XMLElement* curveElement = pair.second->ToXml(doc);
         if (curveElement)
         {
            element->InsertEndChild(curveElement);
         }
      }
   }

   return element;
}

bool CurveList::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   Clear();

   for (const tinyxml2::XMLElement* curveElement = element->FirstChildElement("Curve"); curveElement; curveElement = curveElement->NextSiblingElement("Curve"))
   {
      Curve* curve = new Curve();
      if (curve->FromXml(curveElement))
      {
         if (!curve->GetName().empty() && !Contains(curve->GetName()))
         {
            Add(curve->GetName(), curve);
         }
         else
         {
            delete curve;
         }
      }
      else
      {
         delete curve;
      }
   }

   return true;
}

}