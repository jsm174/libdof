#pragma once

#include "MatrixShiftEffectBase.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

class AnalogAlphaMatrixShiftEffect : public MatrixShiftEffectBase<AnalogAlpha>
{
public:
   AnalogAlphaMatrixShiftEffect();
   virtual ~AnalogAlphaMatrixShiftEffect() = default;

   const AnalogAlpha& GetActiveValue() const { return m_activeValue; }
   void SetActiveValue(const AnalogAlpha& value) { m_activeValue = value; }
   const AnalogAlpha& GetInactiveValue() const { return m_inactiveValue; }
   void SetInactiveValue(const AnalogAlpha& value) { m_inactiveValue = value; }

protected:
   virtual AnalogAlpha GetInactiveValue() override;
   virtual AnalogAlpha GetActiveValue(int triggerValue) override;

private:
   AnalogAlpha m_activeValue;
   AnalogAlpha m_inactiveValue;
};

}