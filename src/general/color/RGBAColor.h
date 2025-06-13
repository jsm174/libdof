#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace DOF
{

class RGBColor;

class RGBAColor
{
private:
   int m_red;
   int m_green;
   int m_blue;
   int m_alpha;

public:
   int GetRed() const { return m_red; }
   void SetRed(int value) { m_red = std::clamp(value, 0, 255); }
   int GetGreen() const { return m_green; }
   void SetGreen(int value) { m_green = std::clamp(value, 0, 255); }
   int GetBlue() const { return m_blue; }
   void SetBlue(int value) { m_blue = std::clamp(value, 0, 255); }
   int GetAlpha() const { return m_alpha; }
   void SetAlpha(int value) { m_alpha = std::clamp(value, 0, 255); }
   RGBAColor Clone() const;

   std::string GetHexColor() const;
   void SetHexColor(const std::string& value);
   RGBAColor GetRGBAColor() const;
   bool SetColor(int red, int green, int blue, int alpha);
   bool SetColor(int red, int green, int blue);
   bool SetColor(const RGBColor& color);
   bool SetColor(const std::string& color);
   void SetRGBA(int red, int green, int blue, int alpha = 255);
   bool IsBlack() const { return m_red == 0 && m_green == 0 && m_blue == 0; }
   bool operator==(const RGBAColor& other) const;
   bool operator!=(const RGBAColor& other) const { return !(*this == other); }
   std::string ToString() const;
   RGBAColor();
   RGBAColor(int brightnessRed, int brightnessGreen, int brightnessBlue);
   RGBAColor(int brightnessRed, int brightnessGreen, int brightnessBlue, int alpha);
   RGBAColor(const std::string& color);
   RGBAColor(const RGBColor& rgbColor);

private:
   bool IsHexString(const std::string& str, int startPos = 0, int length = -1) const;
   int HexToInt(const std::string& hex) const;
   bool IsInteger(const std::string& str) const;
   std::vector<std::string> Split(const std::string& str, char delimiter) const;
};

}