#include "ColorScale.h"
#include "../bitmap/PixelData.h"
#include <algorithm>

namespace DOF
{

ColorScale::ColorScale()
   : m_color(255, 255, 255, 255)
{
}

ColorScale::ColorScale(const RGBAColor& color)
   : m_color(color)
{
}

ColorScale::ColorScale(int red, int green, int blue, int alpha)
   : m_color(red, green, blue, alpha)
{
}

ColorScale::ColorScale(double hue, double saturation, double brightness, int alpha)
{
   UpdateFromHSB(hue, saturation, brightness);
   m_color.SetAlpha(MathExtensions::Limit(alpha, 0, 255));
}

RGBAColor ColorScale::ApplyScale(const RGBAColor& baseColor) const
{

   int r = MathExtensions::Limit((int)((float)baseColor.GetRed() * m_color.GetRed() / 255), 0, 255);
   int g = MathExtensions::Limit((int)((float)baseColor.GetGreen() * m_color.GetGreen() / 255), 0, 255);
   int b = MathExtensions::Limit((int)((float)baseColor.GetBlue() * m_color.GetBlue() / 255), 0, 255);
   int a = MathExtensions::Limit((int)((float)baseColor.GetAlpha() * m_color.GetAlpha() / 255), 0, 255);

   return RGBAColor(r, g, b, a);
}

RGBAColor ColorScale::ApplyScale(const PixelData& pixel) const
{

   RGBAColor baseColor(pixel.red, pixel.green, pixel.blue, pixel.alpha);
   return ApplyScale(baseColor);
}

double ColorScale::GetHue() const
{
   double h, s, br;
   RGBToHSB(m_color.GetRed(), m_color.GetGreen(), m_color.GetBlue(), h, s, br);
   return h;
}

double ColorScale::GetSaturation() const
{
   double h, s, br;
   RGBToHSB(m_color.GetRed(), m_color.GetGreen(), m_color.GetBlue(), h, s, br);
   return s;
}

double ColorScale::GetBrightness() const
{
   double h, s, br;
   RGBToHSB(m_color.GetRed(), m_color.GetGreen(), m_color.GetBlue(), h, s, br);
   return br;
}

void ColorScale::SetHue(double hue)
{
   double h, s, br;
   RGBToHSB(m_color.GetRed(), m_color.GetGreen(), m_color.GetBlue(), h, s, br);
   UpdateFromHSB(hue, s, br);
}

void ColorScale::SetSaturation(double saturation)
{
   double h, s, br;
   RGBToHSB(m_color.GetRed(), m_color.GetGreen(), m_color.GetBlue(), h, s, br);
   UpdateFromHSB(h, saturation, br);
}

void ColorScale::SetBrightness(double brightness)
{
   double h, s, br;
   RGBToHSB(m_color.GetRed(), m_color.GetGreen(), m_color.GetBlue(), h, s, br);
   UpdateFromHSB(h, s, brightness);
}

void ColorScale::UpdateFromHSB(double h, double s, double br)
{
   int r, g, b;
   HSBToRGB(h, s, br, r, g, b);
   m_color.SetRed(r);
   m_color.SetGreen(g);
   m_color.SetBlue(b);
}

RGBAColor ColorScale::BlendColors(const RGBAColor& base, const RGBAColor& overlay, double blendFactor)
{
   blendFactor = MathExtensions::Limit((float)blendFactor, 0.0f, 1.0f);
   double invBlend = 1.0 - blendFactor;

   int r = MathExtensions::Limit((int)(base.GetRed() * invBlend + overlay.GetRed() * blendFactor), 0, 255);
   int g = MathExtensions::Limit((int)(base.GetGreen() * invBlend + overlay.GetGreen() * blendFactor), 0, 255);
   int b = MathExtensions::Limit((int)(base.GetBlue() * invBlend + overlay.GetBlue() * blendFactor), 0, 255);
   int a = MathExtensions::Limit((int)(base.GetAlpha() * invBlend + overlay.GetAlpha() * blendFactor), 0, 255);

   return RGBAColor(r, g, b, a);
}

RGBAColor ColorScale::TintColor(const RGBAColor& base, const RGBAColor& tint, double tintStrength)
{
   tintStrength = MathExtensions::Limit((float)tintStrength, 0.0f, 1.0f);

   return BlendColors(base, tint, tintStrength);
}

RGBAColor ColorScale::ScaleBrightness(const RGBAColor& color, double factor)
{
   factor = MathExtensions::Limit((float)factor, 0.0f, 2.0f);

   int r = MathExtensions::Limit((int)(color.GetRed() * factor), 0, 255);
   int g = MathExtensions::Limit((int)(color.GetGreen() * factor), 0, 255);
   int b = MathExtensions::Limit((int)(color.GetBlue() * factor), 0, 255);

   return RGBAColor(r, g, b, color.GetAlpha());
}

void ColorScale::RGBToHSB(int r, int g, int b, double& h, double& s, double& br)
{
   double rf = r / 255.0;
   double gf = g / 255.0;
   double bf = b / 255.0;

   double maxVal = std::max({ rf, gf, bf });
   double minVal = std::min({ rf, gf, bf });
   double delta = maxVal - minVal;

   br = maxVal;

   if (maxVal == 0.0)
      s = 0.0;
   else
      s = delta / maxVal;

   if (delta == 0.0)
      h = 0.0;
   else if (maxVal == rf)
      h = 60.0 * std::fmod((gf - bf) / delta, 6.0);
   else if (maxVal == gf)
      h = 60.0 * ((bf - rf) / delta + 2.0);
   else
      h = 60.0 * ((rf - gf) / delta + 4.0);

   if (h < 0.0)
      h += 360.0;
}

void ColorScale::HSBToRGB(double h, double s, double br, int& r, int& g, int& b)
{

   h = std::fmod(h, 360.0);
   if (h < 0.0)
      h += 360.0;
   s = MathExtensions::Limit((float)s, 0.0f, 1.0f);
   br = MathExtensions::Limit((float)br, 0.0f, 1.0f);

   double c = br * s;
   double x = c * (1.0 - std::abs(std::fmod(h / 60.0, 2.0) - 1.0));
   double m = br - c;

   double rf, gf, bf;

   if (h >= 0 && h < 60)
   {
      rf = c;
      gf = x;
      bf = 0;
   }
   else if (h >= 60 && h < 120)
   {
      rf = x;
      gf = c;
      bf = 0;
   }
   else if (h >= 120 && h < 180)
   {
      rf = 0;
      gf = c;
      bf = x;
   }
   else if (h >= 180 && h < 240)
   {
      rf = 0;
      gf = x;
      bf = c;
   }
   else if (h >= 240 && h < 300)
   {
      rf = x;
      gf = 0;
      bf = c;
   }
   else
   {
      rf = c;
      gf = 0;
      bf = x;
   }

   r = MathExtensions::Limit((int)((rf + m) * 255), 0, 255);
   g = MathExtensions::Limit((int)((gf + m) * 255), 0, 255);
   b = MathExtensions::Limit((int)((bf + m) * 255), 0, 255);
}

}