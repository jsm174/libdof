#pragma once

#include "../EffectEffectBase.h"

namespace DOF
{

class TableElementData;

class ValueInvertEffect : public EffectEffectBase
{
public:
   ValueInvertEffect() = default;
   virtual ~ValueInvertEffect() = default;

   virtual void Trigger(TableElementData* tableElementData) override;

   virtual std::string GetXmlElementName() const override { return "ValueInvertEffect"; }
};

}