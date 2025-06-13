#pragma once

#include "MatrixBitmapEffectBase.h"
#include "../../general/analog/AnalogAlpha.h"

namespace DOF
{

class AnalogAlphaMatrixBitmapEffect : public MatrixBitmapEffectBase<AnalogAlpha>
{
public:
   AnalogAlphaMatrixBitmapEffect();
   virtual ~AnalogAlphaMatrixBitmapEffect() = default;

   const AnalogAlpha& GetInactiveValue() const { return m_inactiveValue; }
   void SetInactiveValue(const AnalogAlpha& value) { m_inactiveValue = value; }

protected:
   virtual AnalogAlpha GetInactiveValue() override;
   virtual AnalogAlpha GetPixelValue(const PixelData& pixel, int triggerValue) override;

private:
   AnalogAlpha m_inactiveValue;
};

}