#include "RGBColor.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <vector>

namespace DOF
{

RGBColor::RGBColor()
   : m_red(0)
   , m_green(0)
   , m_blue(0)
{
}

RGBColor::RGBColor(int brightnessRed, int brightnessGreen, int brightnessBlue) { SetColor(brightnessRed, brightnessGreen, brightnessBlue); }

RGBColor::RGBColor(const std::string& color) { SetColor(color); }

std::string RGBColor::GetHexColor() const
{
   std::ostringstream oss;
   oss << "#" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << m_red << std::setw(2) << m_green << std::setw(2) << m_blue;
   return oss.str();
}

void RGBColor::SetHexColor(const std::string& value) { SetColor(value); }

bool RGBColor::SetColor(int red, int green, int blue)
{
   SetRed(red);
   SetGreen(green);
   SetBlue(blue);
   return true;
}

bool RGBColor::SetColor(const std::string& color)
{

   if ((color.length() == 6 && IsHexString(color)) || (color.length() == 7 && color[0] == '#' && IsHexString(color, 1, 6)))
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

   return false;
}

bool RGBColor::IsHexString(const std::string& str, int startPos, int length) const
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

int RGBColor::HexToInt(const std::string& hex) const
{
   int result;
   std::istringstream iss(hex);
   iss >> std::hex >> result;
   return result;
}

bool RGBColor::IsInteger(const std::string& str) const
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

std::vector<std::string> RGBColor::Split(const std::string& str, char delimiter) const
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