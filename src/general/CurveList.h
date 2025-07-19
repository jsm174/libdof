#pragma once

#include "Curve.h"
#include "generic/IXmlSerializable.h"
#include <string>
#include <unordered_map>

namespace DOF
{

class CurveList : public IXmlSerializable
{
private:
   std::unordered_map<std::string, Curve*> m_curves;

public:
   CurveList();
   ~CurveList();

   bool Contains(const std::string& name) const;
   Curve* GetByName(const std::string& name) const;
   void Add(const std::string& name, Curve* curve);
   void Remove(const std::string& name);
   void Clear();

   size_t size() const { return m_curves.size(); }
   bool empty() const { return m_curves.empty(); }

   std::unordered_map<std::string, Curve*>::iterator begin() { return m_curves.begin(); }
   std::unordered_map<std::string, Curve*>::iterator end() { return m_curves.end(); }
   std::unordered_map<std::string, Curve*>::const_iterator begin() const { return m_curves.begin(); }
   std::unordered_map<std::string, Curve*>::const_iterator end() const { return m_curves.end(); }

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "CurveList"; }
};

}