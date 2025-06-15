#include "RGBAColor.h"
#include "RGBColor.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <vector>

namespace DOF
{

RGBAColor::RGBAColor()
   : m_red(0)
   , m_green(0)
   , m_blue(0)
   , m_alpha(255)
{
}

RGBAColor::RGBAColor(int brightnessRed, int brightnessGreen, int brightnessBlue) { SetColor(brightnessRed, brightnessGreen, brightnessBlue); }

RGBAColor::RGBAColor(int brightnessRed, int brightnessGreen, int brightnessBlue, int alpha) { SetColor(brightnessRed, brightnessGreen, brightnessBlue, alpha); }

RGBAColor::RGBAColor(const std::string& color) { SetColor(color); }

RGBAColor::RGBAColor(const RGBColor& rgbColor) { SetColor(rgbColor); }

RGBAColor RGBAColor::Clone() const { return RGBAColor(GetHexColor()); }

std::string RGBAColor::GetHexColor() const
{
   std::ostringstream oss;
   oss << "#" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << m_red << std::setw(2) << m_green << std::setw(2) << m_blue << std::setw(2) << m_alpha;
   return oss.str();
}

void RGBAColor::SetHexColor(const std::string& value) { SetColor(value); }

RGBAColor RGBAColor::GetRGBAColor() const { return RGBAColor(m_red, m_green, m_blue, m_alpha); }

bool RGBAColor::SetColor(int red, int green, int blue, int alpha)
{
   SetRed(red);
   SetGreen(green);
   SetBlue(blue);
   SetAlpha(alpha);
   return true;
}

bool RGBAColor::SetColor(int red, int green, int blue)
{
   SetRed(red);
   SetGreen(green);
   SetBlue(blue);
   SetAlpha((red + green + blue > 0) ? 255 : 0);
   return true;
}

bool RGBAColor::SetColor(const RGBColor& color)
{
   SetColor(color.GetRed(), color.GetGreen(), color.GetBlue());
   return true;
}

bool RGBAColor::SetColor(const std::string& color)
{

   if ((color.length() == 8 && IsHexString(color)) || (color.length() == 9 && color[0] == '#' && IsHexString(color, 1)))
   {
      int offset = (color[0] == '#') ? 1 : 0;
      int red = HexToInt(color.substr(0 + offset, 2));
      int green = HexToInt(color.substr(2 + offset, 2));
      int blue = HexToInt(color.substr(4 + offset, 2));
      int alpha = HexToInt(color.substr(6 + offset, 2));
      SetColor(red, green, blue, alpha);
      return true;
   }


   if ((color.length() == 6 && IsHexString(color)) || (color.length() == 7 && color[0] == '#' && IsHexString(color, 1)))
   {
      int offset = (color[0] == '#') ? 1 : 0;
      int red = HexToInt(color.substr(0 + offset, 2));
      int green = HexToInt(color.substr(2 + offset, 2));
      int blue = HexToInt(color.substr(4 + offset, 2));
      SetColor(red, green, blue);
      return true;
   }


   std::vector<std::string> splitColors = Split(color, ',');
   if (splitColors.size() == 3)
   {
      bool colorsOK = true;
      for (const auto& c : splitColors)
      {
         if (!IsInteger(c))
         {
            colorsOK = false;
            break;
         }
      }
      if (colorsOK)
      {
         SetColor(std::stoi(splitColors[0]), std::stoi(splitColors[1]), std::stoi(splitColors[2]));
         return true;
      }
   }
   else if (splitColors.size() == 4)
   {
      bool colorsOK = true;
      for (const auto& c : splitColors)
      {
         if (!IsInteger(c))
         {
            colorsOK = false;
            break;
         }
      }
      if (colorsOK)
      {
         SetColor(std::stoi(splitColors[0]), std::stoi(splitColors[1]), std::stoi(splitColors[2]), std::stoi(splitColors[3]));
         return true;
      }
   }

   return false;
}

void RGBAColor::SetRGBA(int red, int green, int blue, int alpha)
{
   SetRed(red);
   SetGreen(green);
   SetBlue(blue);
   SetAlpha(alpha);
}

bool RGBAColor::operator==(const RGBAColor& other) const { return m_red == other.m_red && m_green == other.m_green && m_blue == other.m_blue && m_alpha == other.m_alpha; }

std::string RGBAColor::ToString() const { return GetHexColor(); }

bool RGBAColor::IsHexString(const std::string& str, int startPos, int length) const
{
   if (length == -1)
      length = static_cast<int>(str.length()) - startPos;

   if (startPos + length > static_cast<int>(str.length()))
      return false;

   for (int i = startPos; i < startPos + length; i++)
   {
      if (!std::isxdigit(str[i]))
         return false;
   }
   return true;
}

int RGBAColor::HexToInt(const std::string& hex) const
{
   int result;
   std::istringstream iss(hex);
   iss >> std::hex >> result;
   return result;
}

bool RGBAColor::IsInteger(const std::string& str) const
{
   if (str.empty())
      return false;

   for (char c : str)
   {
      if (!std::isdigit(c))
         return false;
   }
   return true;
}

std::vector<std::string> RGBAColor::Split(const std::string& str, char delimiter) const
{
   std::vector<std::string> result;
   std::istringstream iss(str);
   std::string token;

   while (std::getline(iss, token, delimiter))
   {
      result.push_back(token);
   }

   return result;
}

}