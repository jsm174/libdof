#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace DOF
{

class RGBColor
{
private:
   int m_red;
   int m_green;
   int m_blue;

public:
   int GetRed() const { return m_red; }
   void SetRed(int value) { m_red = std::clamp(value, 0, 255); }
   int GetGreen() const { return m_green; }
   void SetGreen(int value) { m_green = std::clamp(value, 0, 255); }
   int GetBlue() const { return m_blue; }
   void SetBlue(int value) { m_blue = std::clamp(value, 0, 255); }
   std::string GetHexColor() const;
   void SetHexColor(const std::string& value);
   bool SetColor(int red, int green, int blue);
   bool SetColor(const std::string& color);
   RGBColor();
   RGBColor(int brightnessRed, int brightnessGreen, int brightnessBlue);
   RGBColor(const std::string& color);

private:
   bool IsHexString(const std::string& str, int startPos = 0, int length = -1) const;
   int HexToInt(const std::string& hex) const;
   bool IsInteger(const std::string& str) const;
   std::vector<std::string> Split(const std::string& str, char delimiter) const;
};

}