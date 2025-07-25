#include "ColorList.h"
#include <tinyxml2/tinyxml2.h>

namespace DOF
{

ColorList::ColorList() { }

ColorList::~ColorList() { }

void ColorList::UpdateIndex()
{
   m_nameIndex.clear();
   for (size_t i = 0; i < m_colors.size(); ++i)
   {
      if (!m_colors[i].GetName().empty())
      {
         m_nameIndex[m_colors[i].GetName()] = i;
      }
   }
}

void ColorList::Add(const RGBAColorNamed& color)
{

   if (!color.GetName().empty() && Contains(color.GetName()))
   {
      return;
   }

   m_colors.push_back(color);

   if (!color.GetName().empty())
   {
      m_nameIndex[color.GetName()] = m_colors.size() - 1;
   }
}

bool ColorList::Remove(const std::string& name)
{
   auto it = m_nameIndex.find(name);
   if (it != m_nameIndex.end())
   {
      size_t index = it->second;
      m_colors.erase(m_colors.begin() + index);
      UpdateIndex();
      return true;
   }
   return false;
}

bool ColorList::Remove(size_t index)
{
   if (index < m_colors.size())
   {
      m_colors.erase(m_colors.begin() + index);
      UpdateIndex();
      return true;
   }
   return false;
}

void ColorList::Clear()
{
   m_colors.clear();
   m_nameIndex.clear();
}

RGBAColorNamed* ColorList::Find(const std::string& name)
{
   auto it = m_nameIndex.find(name);
   if (it != m_nameIndex.end())
   {
      return &m_colors[it->second];
   }
   return nullptr;
}

const RGBAColorNamed* ColorList::Find(const std::string& name) const
{
   auto it = m_nameIndex.find(name);
   if (it != m_nameIndex.end())
   {
      return &m_colors[it->second];
   }
   return nullptr;
}

bool ColorList::Contains(const std::string& name) const { return m_nameIndex.find(name) != m_nameIndex.end(); }

void ColorList::Sort()
{
   std::sort(m_colors.begin(), m_colors.end(), [](const RGBAColorNamed& a, const RGBAColorNamed& b) { return a.GetName() < b.GetName(); });
   UpdateIndex();
}

tinyxml2::XMLElement* ColorList::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   for (const auto& color : m_colors)
   {
      tinyxml2::XMLElement* colorElement = color.ToXml(doc);
      if (colorElement)
      {
         element->InsertEndChild(colorElement);
      }
   }

   return element;
}

bool ColorList::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   Clear();

   for (const tinyxml2::XMLElement* colorElement = element->FirstChildElement("RGBAColorNamed"); colorElement; colorElement = colorElement->NextSiblingElement("RGBAColorNamed"))
   {
      RGBAColorNamed color;
      if (color.FromXml(colorElement))
      {
         if (!color.GetName().empty() && !Contains(color.GetName()))
         {
            Add(color);
         }
      }
   }

   return true;
}

}