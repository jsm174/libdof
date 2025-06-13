#pragma once

#include "RGBAColor.h"
#include "../MathExtensions.h"
#include <cmath>

namespace DOF
{

struct PixelData;

class ColorScale
{
public:
   ColorScale();
   ColorScale(const RGBAColor& color);
   ColorScale(int red, int green, int blue, int alpha = 255);


   ColorScale(double hue, double saturation, double brightness, int alpha = 255);

   const RGBAColor& GetColor() const { return m_color; }
   void SetColor(const RGBAColor& value) { m_color = value; }


   RGBAColor ApplyScale(const RGBAColor& baseColor) const;
   RGBAColor ApplyScale(const PixelData& pixel) const;


   double GetHue() const;
   double GetSaturation() const;
   double GetBrightness() const;
   int GetAlpha() const { return m_color.GetAlpha(); }

   void SetHue(double hue);
   void SetSaturation(double saturation);
   void SetBrightness(double brightness);
   void SetAlpha(int alpha) { m_color.SetAlpha(MathExtensions::Limit(alpha, 0, 255)); }


   static RGBAColor BlendColors(const RGBAColor& base, const RGBAColor& overlay, double blendFactor);
   static RGBAColor TintColor(const RGBAColor& base, const RGBAColor& tint, double tintStrength);
   static RGBAColor ScaleBrightness(const RGBAColor& color, double factor);


   static void RGBToHSB(int r, int g, int b, double& h, double& s, double& br);
   static void HSBToRGB(double h, double s, double br, int& r, int& g, int& b);

private:
   RGBAColor m_color;

   void UpdateFromHSB(double h, double s, double br);
};

}