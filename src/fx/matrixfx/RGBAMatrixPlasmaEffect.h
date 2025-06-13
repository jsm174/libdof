#pragma once

#include "MatrixPlasmaEffectBase.h"
#include "../../general/color/RGBAColor.h"

namespace DOF
{

class RGBAMatrixPlasmaEffect : public MatrixPlasmaEffectBase<RGBAColor>
{
public:
   RGBAMatrixPlasmaEffect();
   virtual ~RGBAMatrixPlasmaEffect() = default;

   const RGBAColor& GetActiveColor() const { return m_activeColor; }
   void SetActiveColor(const RGBAColor& value) { m_activeColor = value; }
   const RGBAColor& GetInactiveColor() const { return m_inactiveColor; }
   void SetInactiveColor(const RGBAColor& value) { m_inactiveColor = value; }
   const RGBAColor& GetSecondaryColor() const { return m_secondaryColor; }
   void SetSecondaryColor(const RGBAColor& value) { m_secondaryColor = value; }

protected:
   virtual RGBAColor GetInactiveValue() override;
   virtual RGBAColor GetPlasmaValue(int triggerValue, double intensity) override;

private:
   RGBAColor m_activeColor;
   RGBAColor m_inactiveColor;
   RGBAColor m_secondaryColor;
};

}