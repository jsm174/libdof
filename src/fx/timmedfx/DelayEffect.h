#pragma once

#include "../EffectEffectBase.h"
#include <functional>

namespace DOF
{

class TableElementData;

class DelayEffect : public EffectEffectBase
{
public:
   DelayEffect();
   virtual ~DelayEffect() = default;

   int GetDelayMs() const { return m_delayMs; }
   void SetDelayMs(int value);
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Init(Table* table) override;
   virtual void Finish() override;

   virtual std::string GetXmlElementName() const override { return "DelayEffect"; }

private:
   void AfterDelay(TableElementData* data);

   int m_delayMs;
   std::function<void()> m_afterDelayCallback;
};

}