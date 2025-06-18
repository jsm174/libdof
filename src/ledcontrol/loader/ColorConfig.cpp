#include "ColorConfig.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include "../../general/MathExtensions.h"
#include "../../general/color/RGBAColor.h"
#include <stdexcept>

namespace DOF
{

RGBAColorNamed ColorConfig::GetCabinetColor() const { return RGBAColorNamed(m_name, m_red, m_green, m_blue, m_alpha); }

void ColorConfig::ParseLedcontrolData(const std::string& colorConfigDataLine, bool throwExceptions)
{
   std::vector<std::string> nameValues = StringExtensions::Split(colorConfigDataLine, { '=' });
   if (nameValues.size() == 2)
   {
      std::vector<std::string> values = StringExtensions::Split(nameValues[1], { ',' });
      if (values.size() == 3 && StringExtensions::IsInteger(values[0]) && StringExtensions::IsInteger(values[1]) && StringExtensions::IsInteger(values[2]))
      {
         m_name = nameValues[0];
         m_red = static_cast<int>(MathExtensions::Limit(StringExtensions::ToInteger(values[0]), 0, 48) * 5.3125);
         m_green = static_cast<int>(MathExtensions::Limit(StringExtensions::ToInteger(values[1]), 0, 48) * 5.3125);
         m_blue = static_cast<int>(MathExtensions::Limit(StringExtensions::ToInteger(values[2]), 0, 48) * 5.3125);
         m_alpha = (m_red + m_green + m_blue == 0 ? 0 : 255);
         return;
      }
      else if (values.size() == 4 && StringExtensions::IsInteger(values[0]) && StringExtensions::IsInteger(values[1]) && StringExtensions::IsInteger(values[2])
         && StringExtensions::IsInteger(values[3]))
      {
         m_name = nameValues[0];
         m_red = static_cast<int>(MathExtensions::Limit(StringExtensions::ToInteger(values[0]), 0, 48) * 5.3125);
         m_green = static_cast<int>(MathExtensions::Limit(StringExtensions::ToInteger(values[1]), 0, 48) * 5.3125);
         m_blue = static_cast<int>(MathExtensions::Limit(StringExtensions::ToInteger(values[2]), 0, 48) * 5.3125);
         m_alpha = static_cast<int>(MathExtensions::Limit(StringExtensions::ToInteger(values[3]), 0, 48) * 5.3125);
         return;
      }
      else if (StringExtensions::StartsWith(nameValues[1], "#") && (nameValues[1].length() == 7 || nameValues[1].length() == 9) && StringExtensions::IsHexString(nameValues[1].substr(1)))
      {
         RGBAColor c;
         if (c.SetColor(nameValues[1]))
         {
            m_name = nameValues[0];
            m_red = c.GetRed();
            m_green = c.GetGreen();
            m_blue = c.GetBlue();
            m_alpha = c.GetAlpha();
            return;
         }
      }
   }
   Log::Warning(StringExtensions::Build("Line {0} has a unknown structure or contains wrong data.", colorConfigDataLine));
   if (throwExceptions)
      throw std::runtime_error(StringExtensions::Build("Line {0} has a unknown structure or contains wrong data.", colorConfigDataLine));
   return;
}

ColorConfig::ColorConfig()
   : m_red(0)
   , m_green(0)
   , m_blue(0)
   , m_alpha(255)
{
}

ColorConfig::ColorConfig(const std::string& colorConfigDataLine, bool throwExceptions)
   : m_red(0)
   , m_green(0)
   , m_blue(0)
   , m_alpha(255)
{
   ParseLedcontrolData(colorConfigDataLine, throwExceptions);
}

ColorConfig::~ColorConfig() { }

}