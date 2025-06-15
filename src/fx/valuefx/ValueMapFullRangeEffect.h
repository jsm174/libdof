#pragma once

#include "../EffectEffectBase.h"
#include <unordered_map>
#include <string>

namespace DOF
{

class TableElementData;

class ValueMapFullRangeEffect : public EffectEffectBase
{
public:
   ValueMapFullRangeEffect() = default;
   virtual ~ValueMapFullRangeEffect() = default;

   virtual void Trigger(TableElementData* tableElementData) override;

   virtual std::string GetXmlElementName() const override { return "ValueMapFullRangeEffect"; }

private:
   std::unordered_map<std::string, int> m_previousState;
};

}