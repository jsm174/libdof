#pragma once

#include "MatrixBitmapEffectBase.h"
#include "../../general/color/ColorScale.h"
#include "../../general/color/RGBAColor.h"

namespace DOF
{

class TableElementData;

template <typename MatrixElementType> class MatrixColorScaleEffectBase : public MatrixBitmapEffectBase<MatrixElementType>
{
public:
   MatrixColorScaleEffectBase();
   virtual ~MatrixColorScaleEffectBase() = default;

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
   virtual MatrixElementType GetColorScaledPixelValue(const PixelData& pixel, int triggerValue) = 0;
   virtual MatrixElementType GetPixelValue(const PixelData& pixel, int triggerValue) override;
   RGBAColor ApplyColorScaling(const PixelData& pixel) const;

   ColorScale m_colorScale;
   ColorScaleMode m_colorScaleMode;
   double m_colorScaleStrength;
};

template <typename MatrixElementType>
MatrixColorScaleEffectBase<MatrixElementType>::MatrixColorScaleEffectBase()
   : m_colorScale(255, 255, 255, 255)
   , m_colorScaleMode(ColorScaleMode::Multiply)
   , m_colorScaleStrength(1.0)
{
}

template <typename MatrixElementType> MatrixElementType MatrixColorScaleEffectBase<MatrixElementType>::GetPixelValue(const PixelData& pixel, int triggerValue)
{
   return GetColorScaledPixelValue(pixel, triggerValue);
}

template <typename MatrixElementType> RGBAColor MatrixColorScaleEffectBase<MatrixElementType>::ApplyColorScaling(const PixelData& pixel) const
{
   RGBAColor baseColor(pixel.red, pixel.green, pixel.blue, pixel.alpha);

   switch (m_colorScaleMode)
   {
   case ColorScaleMode::Multiply: return m_colorScale.ApplyScale(baseColor);

   case ColorScaleMode::Tint: return ColorScale::TintColor(baseColor, m_colorScale.GetColor(), m_colorScaleStrength);

   case ColorScaleMode::Replace:
   {
      double brightness = (pixel.red * 0.299 + pixel.green * 0.587 + pixel.blue * 0.114) / 255.0;
      RGBAColor scaledColor = ColorScale::ScaleBrightness(m_colorScale.GetColor(), brightness);
      return ColorScale::BlendColors(baseColor, scaledColor, m_colorScaleStrength);
   }

   default: return baseColor;
   }
}

}