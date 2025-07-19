#pragma once

#include "RGBAColorNamed.h"
#include "../generic/IXmlSerializable.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

namespace DOF
{

class ColorList : public IXmlSerializable
{
private:
   std::vector<RGBAColorNamed> m_colors;
   std::unordered_map<std::string, size_t> m_nameIndex;

   void UpdateIndex();

public:
   ColorList();
   ~ColorList();


   void Add(const RGBAColorNamed& color);
   bool Remove(const std::string& name);
   bool Remove(size_t index);
   void Clear();

   size_t Size() const { return m_colors.size(); }
   bool Empty() const { return m_colors.empty(); }


   RGBAColorNamed& operator[](size_t index) { return m_colors[index]; }
   const RGBAColorNamed& operator[](size_t index) const { return m_colors[index]; }


   RGBAColorNamed* Find(const std::string& name);
   const RGBAColorNamed* Find(const std::string& name) const;
   bool Contains(const std::string& name) const;


   std::vector<RGBAColorNamed>::iterator begin() { return m_colors.begin(); }
   std::vector<RGBAColorNamed>::iterator end() { return m_colors.end(); }
   std::vector<RGBAColorNamed>::const_iterator begin() const { return m_colors.begin(); }
   std::vector<RGBAColorNamed>::const_iterator end() const { return m_colors.end(); }


   void Sort();

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "ColorList"; }
};

}