#pragma once

#include <string>
#include "../../general/color/RGBAColorNamed.h"

#include "DOF/DOF.h"

namespace DOF
{

class ColorConfig
{
public:
   ColorConfig();
   ColorConfig(const std::string& colorConfigDataLine, bool throwExceptions = false);
   ~ColorConfig();

   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name) { m_name = name; }
   int GetRed() const { return m_red; }
   void SetRed(int red) { m_red = red; }
   int GetGreen() const { return m_green; }
   void SetGreen(int green) { m_green = green; }
   int GetBlue() const { return m_blue; }
   void SetBlue(int blue) { m_blue = blue; }
   int GetAlpha() const { return m_alpha; }
   void SetAlpha(int alpha) { m_alpha = alpha; }
   void ParseLedcontrolData(const std::string& colorConfigDataLine, bool throwExceptions = false);
   RGBAColorNamed GetCabinetColor() const;

private:
   std::string m_name;
   int m_red;
   int m_green;
   int m_blue;
   int m_alpha;
};

}