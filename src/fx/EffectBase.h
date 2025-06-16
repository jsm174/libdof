#pragma once

#include "../general/generic/NamedItemBase.h"
#include "IEffect.h"

namespace DOF
{

class Table;
class TableElementData;

class EffectBase : public NamedItemBase, public IEffect
{
public:
   EffectBase();
   virtual ~EffectBase() = default;


   const std::string& GetName() const override { return NamedItemBase::GetName(); }
   void SetName(const std::string& name) override { NamedItemBase::SetName(name); }


   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override = 0;

   virtual void Trigger(TableElementData* tableElementData) override = 0;
   virtual void Init(Table* table) override = 0;
   virtual void Finish() override;

protected:
   virtual void SerializeToXml(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& doc) const;
   virtual void DeserializeFromXml(const tinyxml2::XMLElement* element);
};

}