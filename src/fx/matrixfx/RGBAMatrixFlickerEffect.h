#pragma once

#include "MatrixFlickerEffectBase.h"
#include "../../general/color/RGBAColor.h"

namespace DOF
{

class RGBAMatrixFlickerEffect : public MatrixFlickerEffectBase<RGBAColor>
{
public:
   RGBAMatrixFlickerEffect();
   virtual ~RGBAMatrixFlickerEffect() = default;

   const RGBAColor& GetActiveColor() const { return m_activeColor; }
   void SetActiveColor(const RGBAColor& value) { m_activeColor = value; }
   const RGBAColor& GetInactiveColor() const { return m_inactiveColor; }
   void SetInactiveColor(const RGBAColor& value) { m_inactiveColor = value; }

protected:
   virtual RGBAColor GetInactiveValue() override;
   virtual RGBAColor GetActiveValue(int triggerValue) override;

private:
   RGBAColor m_activeColor;
   RGBAColor m_inactiveColor;
};

}