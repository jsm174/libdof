#pragma once

#include "MatrixBitmapAnimationEffectBase.h"
#include "../../general/color/RGBAColor.h"
#include "../../general/color/ColorScale.h"
#include "../../general/MathExtensions.h"

namespace DOF
{

class RGBAMatrixColorScaleBitmapAnimationEffect : public MatrixBitmapAnimationEffectBase<RGBAColor>
{
public:
   RGBAMatrixColorScaleBitmapAnimationEffect();
   virtual ~RGBAMatrixColorScaleBitmapAnimationEffect() = default;

   const RGBAColor& GetInactiveColor() const { return m_inactiveColor; }
   void SetInactiveColor(const RGBAColor& value) { m_inactiveColor = value; }
   const ColorScale& GetColorScale() const { return m_colorScale; }
   void SetColorScale(const ColorScale& value) { m_colorScale = value; }
   const RGBAColor& GetColorScaleColor() const { return m_colorScale.GetColor(); }
   void SetColorScaleColor(const RGBAColor& value) { m_colorScale.SetColor(value); }
   double GetHue() const { return m_colorScale.GetHue(); }
   void SetHue(double value) { m_colorScale.SetHue(value); }
   double GetSaturation() const { return m_colorScale.GetSaturation(); }
   void SetSaturation(double value) { m_colorScale.SetSaturation(value); }
   double GetBrightness() const { return m_colorScale.GetBrightness(); }
   void SetBrightness(double value) { m_colorScale.SetBrightness(value); }
   int GetColorScaleAlpha() const { return m_colorScale.GetAlpha(); }
   void SetColorScaleAlpha(int value) { m_colorScale.SetAlpha(value); }

   enum class ColorScaleMode
   {
      Multiply,
      Tint,
      Replace
   };

   ColorScaleMode GetColorScaleMode() const { return m_colorScaleMode; }
   void SetColorScaleMode(ColorScaleMode value) { m_colorScaleMode = value; }
   double GetColorScaleStrength() const { return m_colorScaleStrength; }
   void SetColorScaleStrength(double value) { m_colorScaleStrength = MathExtensions::Limit((float)value, 0.0f, 1.0f); }

protected:
   virtual RGBAColor GetInactiveValue() override;
   virtual RGBAColor GetPixelValue(const PixelData& pixel, int triggerValue) override;

private:
   RGBAColor m_inactiveColor;
   ColorScale m_colorScale;
   ColorScaleMode m_colorScaleMode;
   double m_colorScaleStrength;

   RGBAColor ApplyColorScaling(const PixelData& pixel) const;
};

}