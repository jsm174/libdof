#pragma once

#include <unordered_map>
#include <string>
#include "DOF/DOF.h"
#include "../general/generic/IXmlSerializable.h"

namespace DOF
{

class IEffect;
class Table;

class EffectList : public std::unordered_map<std::string, IEffect*>, public IXmlSerializable
{
public:
   EffectList() = default;
   ~EffectList() { Finish(); }

   void Finish();
   void Init(Table* pTable);


   virtual XMLElement* ToXml(XMLDocument& doc) const override;
   virtual bool FromXml(const XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "Effects"; }


   void Add(IEffect* effect);
   bool Contains(const std::string& name) const;
};

}
