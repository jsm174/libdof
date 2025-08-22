#include "TableConfigSetting.h"
#include "ColorConfig.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include "../../general/MathExtensions.h"
#include "../../table/TableElementTypeEnum.h"
#include <algorithm>

namespace DOF
{

TableConfigSetting::TableConfigSetting()
   : m_outputControl(OutputControlEnum::Controlled)
   , m_colorConfig(nullptr)
   , m_outputType(OutputTypeEnum::AnalogOutput)
   , m_durationMs(-1)
   , m_minDurationMs(0)
   , m_maxDurationMs(0)
   , m_extDurationMs(0)
   , m_intensity(255)
   , m_fadingUpDurationMs(0)
   , m_fadingDownDurationMs(0)
   , m_blink(0)
   , m_blinkIntervalMs(0)
   , m_blinkPulseWidth(50)
   , m_invert(false)
   , m_noBool(false)
   , m_waitDurationMs(0)
   , m_layer(-1)
   , m_isArea(false)
   , m_blinkIntervalMsNested(0)
   , m_blinkPulseWidthNested(50)
   , m_blinkLow(0)
   , m_areaLeft(0)
   , m_areaTop(0)
   , m_areaWidth(100)
   , m_areaHeight(100)
   , m_areaSpeed(100)
   , m_areaAcceleration(0)
   , m_areaFlickerDensity(0)
   , m_areaFlickerMinDurationMs(0)
   , m_areaFlickerMaxDurationMs(0)
   , m_areaFlickerFadeDurationMs(0)
   , m_areaDirection(MatrixShiftDirectionEnum::Left)
   , m_hasAreaDirection(false)
   , m_isBitmap(false)
   , m_areaBitmapTop(0)
   , m_areaBitmapLeft(0)
   , m_areaBitmapWidth(-1)
   , m_areaBitmapHeight(-1)
   , m_areaBitmapFrame(0)
   , m_areaBitmapAnimationStepSize(1)
   , m_areaBitmapAnimationStepCount(0)
   , m_areaBitmapAnimationFrameDuration(30)
   , m_areaBitmapAnimationDirection(MatrixAnimationStepDirectionEnum::Frame)
   , m_areaBitmapAnimationBehaviour(AnimationBehaviourEnum::Loop)
   , m_isPlasma(false)
   , m_plasmaSpeed(100)
   , m_plasmaDensity(100)
   , m_colorConfig2(nullptr)
{
}

TableConfigSetting::TableConfigSetting(const std::string& settingData)
   : m_outputControl(OutputControlEnum::Controlled)
   , m_colorConfig(nullptr)
   , m_outputType(OutputTypeEnum::AnalogOutput)
   , m_durationMs(-1)
   , m_minDurationMs(0)
   , m_maxDurationMs(0)
   , m_extDurationMs(0)
   , m_intensity(255)
   , m_fadingUpDurationMs(0)
   , m_fadingDownDurationMs(0)
   , m_blink(0)
   , m_blinkIntervalMs(0)
   , m_blinkPulseWidth(50)
   , m_invert(false)
   , m_noBool(false)
   , m_waitDurationMs(0)
   , m_layer(-1)
   , m_isArea(false)
   , m_blinkIntervalMsNested(0)
   , m_blinkPulseWidthNested(50)
   , m_blinkLow(0)
   , m_areaLeft(0)
   , m_areaTop(0)
   , m_areaWidth(100)
   , m_areaHeight(100)
   , m_areaSpeed(100)
   , m_areaAcceleration(0)
   , m_areaFlickerDensity(0)
   , m_areaFlickerMinDurationMs(0)
   , m_areaFlickerMaxDurationMs(0)
   , m_areaFlickerFadeDurationMs(0)
   , m_areaDirection(MatrixShiftDirectionEnum::Left)
   , m_hasAreaDirection(false)
   , m_isBitmap(false)
   , m_areaBitmapTop(0)
   , m_areaBitmapLeft(0)
   , m_areaBitmapWidth(-1)
   , m_areaBitmapHeight(-1)
   , m_areaBitmapFrame(0)
   , m_areaBitmapAnimationStepSize(1)
   , m_areaBitmapAnimationStepCount(0)
   , m_areaBitmapAnimationFrameDuration(30)
   , m_areaBitmapAnimationDirection(MatrixAnimationStepDirectionEnum::Frame)
   , m_areaBitmapAnimationBehaviour(AnimationBehaviourEnum::Loop)
   , m_isPlasma(false)
   , m_plasmaSpeed(100)
   , m_plasmaDensity(100)
   , m_colorConfig2(nullptr)
{
   ParseSettingData(settingData);
}

TableConfigSetting::~TableConfigSetting() { }

void TableConfigSetting::ParseSettingData(const std::string& settingData)
{
   std::string s = StringExtensions::Trim(StringExtensions::ToUpper(settingData));

   if (StringExtensions::StartsWith(s, "("))
   {

      int bracketCnt = 1;
      int closingBracketPos = -1;
      for (size_t i = 1; i < s.length(); i++)
      {
         if (s[i] == '(')
            bracketCnt++;
         else if (s[i] == ')')
            bracketCnt--;

         if (bracketCnt == 0)
         {
            closingBracketPos = (int)i;
            break;
         }
      }

      if (closingBracketPos > 0)
      {
         m_condition = s.substr(0, closingBracketPos + 1);
         m_outputControl = OutputControlEnum::Condition;
         s = StringExtensions::Trim(s.substr(m_condition.length()));
      }
      else
      {
         Log::Warning(StringExtensions::Build("No closing bracket found for condition in setting {0}.", s));
         throw std::runtime_error(StringExtensions::Build("No closing bracket found for condition in setting {0}.", s));
      }
   }
   else
   {

      if (s.length() == 0)
      {
         Log::Warning("No data to parse.");
         throw std::runtime_error("No data to parse.");
      }

      int triggerEndPos = -1;
      char lastChar = 0;
      for (size_t i = 0; i < s.length() - 1; i++)
      {
         if (s[i] == ' ' && lastChar != '|' && lastChar != 0)
         {
            triggerEndPos = (int)i;
            break;
         }
         if (s[i] != ' ')
         {
            lastChar = s[i];
         }
      }
      if (triggerEndPos == -1)
         triggerEndPos = (int)s.length();

      std::string trigger = StringExtensions::Trim(s.substr(0, triggerEndPos));

      bool parseOK = true;
      std::string triggerUpper = StringExtensions::ToUpper(trigger);
      if (triggerUpper == "ON" || triggerUpper == "1")
      {
         m_outputControl = OutputControlEnum::FixedOn;
      }
      else if (triggerUpper == "OFF" || triggerUpper == "0")
      {
         m_outputControl = OutputControlEnum::FixedOff;
      }
      else if (triggerUpper == "B" || triggerUpper == "BLINK")
      {
         m_outputControl = OutputControlEnum::FixedOn;
         m_blink = -1;
         m_blinkIntervalMs = 1000;
      }
      else
      {

         std::vector<std::string> tableElements = StringExtensions::Split(trigger, { '|' });
         for (const std::string& element : tableElements)
         {
            std::string e = StringExtensions::Trim(element);
            if (e.length() > 1)
            {
               if (e[0] == (char)TableElementTypeEnum::NamedElement)
               {

                  bool validNamed = true;
                  for (size_t j = 1; j < e.length(); j++)
                  {
                     char c = e[j];
                     if (!std::isalnum(c) && c != '_')
                     {
                        validNamed = false;
                        break;
                     }
                  }
                  if (!validNamed)
                  {
                     parseOK = false;
                     break;
                  }
               }
               else
               {

                  bool validType = false;
                  TableElementTypeEnum elementType = (TableElementTypeEnum)e[0];
                  switch (elementType)
                  {
                  case TableElementTypeEnum::Lamp:
                  case TableElementTypeEnum::Switch:
                  case TableElementTypeEnum::Solenoid:
                  case TableElementTypeEnum::GIString:
                  case TableElementTypeEnum::Mech:
                  case TableElementTypeEnum::GetMech:
                  case TableElementTypeEnum::EMTable:
                  case TableElementTypeEnum::LED:
                  case TableElementTypeEnum::Score:
                  case TableElementTypeEnum::ScoreDigit: validType = true; break;
                  case TableElementTypeEnum::NamedElement:
                  case TableElementTypeEnum::Unknown:
                  default: validType = false; break;
                  }

                  if (validType && StringExtensions::IsInteger(e.substr(1)))
                  {
                  }
                  else
                  {
                     Log::Write(StringExtensions::Build("Failed: {0}", e));
                     parseOK = false;
                     break;
                  }
               }
            }
            else
            {
               parseOK = false;
               break;
            }
         }

         if (parseOK)
         {
            m_outputControl = OutputControlEnum::Controlled;
            m_tableElement = trigger;
         }
      }

      if (!parseOK)
      {
         Log::Warning(StringExtensions::Build("Cant parse the trigger part {0} of the ledcontrol table config setting {1}.", trigger, settingData));
         throw std::runtime_error(StringExtensions::Build("Cant parse the part {0} of the ledcontrol table config setting {1}.", trigger, settingData));
      }

      s = StringExtensions::Trim(s.substr(trigger.length()));
   }

   std::vector<std::string> parts = StringExtensions::Split(s, { ' ' });

   int integerCnt = 0;
   size_t partNr = 0;

   while (parts.size() > partNr)
   {
      std::string part = parts[partNr];
      std::string partUpper = StringExtensions::ToUpper(part);

      if (partUpper == "BLINK")
      {
         m_blink = -1;
         m_blinkIntervalMs = 1000;
      }
      else if (partUpper == "INVERT")
      {
         m_invert = true;
      }
      else if (partUpper == "NOBOOL")
      {
         m_noBool = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "APS" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_plasmaSpeed = StringExtensions::ToInteger(partUpper.substr(3));
         m_isPlasma = true;
         m_isArea = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "APD" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_plasmaDensity = StringExtensions::ToInteger(partUpper.substr(3));
         m_isPlasma = true;
         m_isArea = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "APC")
      {
         m_colorName2 = part.substr(3);
         m_isPlasma = true;
         m_isArea = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "SHP")
      {
         m_shapeName = StringExtensions::Trim(part.substr(3));
         m_isArea = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "ABT" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_areaBitmapTop = StringExtensions::ToInteger(partUpper.substr(3));
         m_isArea = true;
         m_isBitmap = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "ABL" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_areaBitmapLeft = StringExtensions::ToInteger(partUpper.substr(3));
         m_isArea = true;
         m_isBitmap = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "ABW" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_areaBitmapWidth = StringExtensions::ToInteger(partUpper.substr(3));
         m_isArea = true;
         m_isBitmap = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "ABH" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_areaBitmapHeight = StringExtensions::ToInteger(partUpper.substr(3));
         m_isArea = true;
         m_isBitmap = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "ABF" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_areaBitmapFrame = StringExtensions::ToInteger(partUpper.substr(3));
         m_isArea = true;
         m_isBitmap = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "AAS" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_areaBitmapAnimationStepSize = StringExtensions::ToInteger(partUpper.substr(3));
         m_isArea = true;
         m_isBitmap = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "AAC" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_areaBitmapAnimationStepCount = StringExtensions::ToInteger(partUpper.substr(3));
         m_isArea = true;
         m_isBitmap = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "AAF" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         int fps = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(3)), 10, INT_MAX);
         m_areaBitmapAnimationFrameDuration = 1000 / fps;
         m_isArea = true;
         m_isBitmap = true;
      }
      else if (partUpper.length() == 4 && partUpper.substr(0, 3) == "AAD")
      {
         char dirChar = partUpper[3];
         m_areaBitmapAnimationDirection = (MatrixAnimationStepDirectionEnum)dirChar;
         m_isArea = true;
         m_isBitmap = true;
      }
      else if (partUpper.length() == 4 && partUpper.substr(0, 3) == "AAB")
      {
         char behaviorChar = partUpper[3];
         m_areaBitmapAnimationBehaviour = (AnimationBehaviourEnum)behaviorChar;
         m_isArea = true;
         m_isBitmap = true;
      }
      else if (partUpper.length() > 5 && partUpper.substr(0, 5) == "AFDEN" && StringExtensions::IsInteger(partUpper.substr(5)))
      {
         m_areaFlickerDensity = StringExtensions::ToInteger(partUpper.substr(5));
         m_isArea = true;
      }
      else if (partUpper.length() > 5 && partUpper.substr(0, 5) == "AFMIN" && StringExtensions::IsInteger(partUpper.substr(5)))
      {
         m_areaFlickerMinDurationMs = StringExtensions::ToInteger(partUpper.substr(5));
         m_isArea = true;
      }
      else if (partUpper.length() > 5 && partUpper.substr(0, 5) == "AFMAX" && StringExtensions::IsInteger(partUpper.substr(5)))
      {
         m_areaFlickerMaxDurationMs = StringExtensions::ToInteger(partUpper.substr(5));
         m_isArea = true;
      }
      else if (partUpper.length() > 6 && partUpper.substr(0, 6) == "AFFADE" && StringExtensions::IsInteger(partUpper.substr(6)))
      {
         m_areaFlickerFadeDurationMs = StringExtensions::ToInteger(partUpper.substr(6));
         m_isArea = true;
      }
      else if (partUpper.length() > 2 && partUpper.substr(0, 2) == "AT" && StringExtensions::IsInteger(partUpper.substr(2)))
      {
         m_areaTop = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(2)), 0, 100);
         m_isArea = true;
      }
      else if (partUpper.length() > 2 && partUpper.substr(0, 2) == "AL" && StringExtensions::IsInteger(partUpper.substr(2)))
      {
         m_areaLeft = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(2)), 0, 100);
         m_isArea = true;
      }
      else if (partUpper.length() > 2 && partUpper.substr(0, 2) == "AW" && StringExtensions::IsInteger(partUpper.substr(2)))
      {
         m_areaWidth = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(2)), 0, 100);
         m_isArea = true;
      }
      else if (partUpper.length() > 2 && partUpper.substr(0, 2) == "AH" && StringExtensions::IsInteger(partUpper.substr(2)))
      {
         m_areaHeight = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(2)), 0, 100);
         m_isArea = true;
      }
      else if (partUpper.length() > 2 && partUpper.substr(0, 2) == "AA" && StringExtensions::IsInteger(partUpper.substr(2)))
      {
         m_areaAcceleration = StringExtensions::ToInteger(partUpper.substr(2));
         m_isArea = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "ASA" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_areaAcceleration = StringExtensions::ToInteger(partUpper.substr(3));
         m_isArea = true;
      }
      else if (partUpper.length() > 2 && partUpper.substr(0, 2) == "AS" && StringExtensions::IsInteger(partUpper.substr(2)))
      {
         m_areaSpeed = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(2)), 1, 10000);
         m_isArea = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "ASS" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_areaSpeed = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(3)), 1, 10000);
         m_isArea = true;
      }
      else if (partUpper.length() > 5 && partUpper.substr(0, 3) == "ASS" && StringExtensions::EndsWith(partUpper, "MS")
         && StringExtensions::IsInteger(partUpper.substr(3, partUpper.length() - 5)))
      {
         int msValue = StringExtensions::ToInteger(partUpper.substr(3, partUpper.length() - 5));
         m_areaSpeed = MathExtensions::Limit((int)((double)100000 / msValue), 10, 100000);
         m_isArea = true;
      }
      else if (partUpper.length() == 3 && partUpper.substr(0, 2) == "AD")
      {
         char dirChar = partUpper[2];
         m_areaDirection = (MatrixShiftDirectionEnum)dirChar;
         m_hasAreaDirection = true;
         m_isArea = true;
      }
      else if (partUpper.length() == 4 && partUpper.substr(0, 3) == "ASD")
      {
         char dirChar = partUpper[3];
         m_areaDirection = (MatrixShiftDirectionEnum)dirChar;
         m_hasAreaDirection = true;
         m_isArea = true;
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "MAX" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_maxDurationMs = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(3)), 0, INT_MAX);
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "BNP" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_blinkIntervalMsNested = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(3)), 0, INT_MAX);
      }
      else if (partUpper.length() > 4 && partUpper.substr(0, 4) == "BNPW" && StringExtensions::IsInteger(partUpper.substr(4)))
      {
         m_blinkPulseWidthNested = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(4)), 1, 99);
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "BPW" && StringExtensions::IsInteger(partUpper.substr(3)))
      {
         m_blinkPulseWidth = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(3)), 1, 99);
      }
      else if (partUpper.length() > 3 && partUpper.substr(0, 3) == "BL#" && StringExtensions::IsHexString(partUpper.substr(3)))
      {
         m_blinkLow = MathExtensions::Limit(StringExtensions::HexToInt(partUpper.substr(3)), 0, 255);
      }
      else if (partUpper.length() > 2 && partUpper.substr(0, 2) == "BL" && StringExtensions::IsInteger(partUpper.substr(2)))
      {
         m_blinkLow = (int)(((double)MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(2)), 0, 48)) * 5.3125);
      }
      else if (partUpper.length() > 1 && partUpper.substr(0, 1) == "E" && StringExtensions::IsInteger(partUpper.substr(1)))
      {
         m_extDurationMs = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(1)), 0, INT_MAX);
      }
      else if (partUpper.length() > 2 && partUpper.substr(0, 2) == "I#" && StringExtensions::IsHexString(partUpper.substr(2)))
      {
         m_intensity = MathExtensions::Limit(StringExtensions::HexToInt(partUpper.substr(2)), 0, 255);
      }
      else if (partUpper.length() > 1 && partUpper.substr(0, 1) == "I" && StringExtensions::IsInteger(partUpper.substr(1)))
      {
         m_intensity = (int)(((double)MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(1)), 0, 48)) * 5.3125);
      }
      else if (partUpper.length() > 1 && partUpper.substr(0, 1) == "L" && StringExtensions::IsInteger(partUpper.substr(1)))
      {
         m_layer = StringExtensions::ToInteger(partUpper.substr(1));
      }
      else if (partUpper.length() > 1 && partUpper.substr(0, 1) == "W" && StringExtensions::IsInteger(partUpper.substr(1)))
      {
         m_waitDurationMs = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(1)), 0, INT_MAX);
      }
      else if (partUpper.length() > 1 && partUpper.substr(0, 1) == "M" && StringExtensions::IsInteger(partUpper.substr(1)))
      {
         m_minDurationMs = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(1)), 0, INT_MAX);
      }
      else if (partUpper.length() > 1 && partUpper.substr(0, 1) == "F" && StringExtensions::IsInteger(partUpper.substr(1)))
      {
         m_fadingUpDurationMs = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(1)), 0, INT_MAX);
         m_fadingDownDurationMs = m_fadingUpDurationMs;
      }
      else if (partUpper.length() > 2 && partUpper.substr(0, 2) == "FU" && StringExtensions::IsInteger(partUpper.substr(2)))
      {
         m_fadingUpDurationMs = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(2)), 0, INT_MAX);
      }
      else if (partUpper.length() > 2 && partUpper.substr(0, 2) == "FD" && StringExtensions::IsInteger(partUpper.substr(2)))
      {
         m_fadingDownDurationMs = MathExtensions::Limit(StringExtensions::ToInteger(partUpper.substr(2)), 0, INT_MAX);
      }
      else if (StringExtensions::IsInteger(part))
      {
         switch (integerCnt)
         {
         case 0:
            if (m_blink == -1)
            {
               m_blinkIntervalMs = MathExtensions::Limit(StringExtensions::ToInteger(part), 1, INT_MAX);
               m_durationMs = 0;
            }
            else
            {
               m_durationMs = MathExtensions::Limit(StringExtensions::ToInteger(part), 1, INT_MAX);
            }
            break;
         case 1:
            if (m_blink != -1)
            {
               m_blink = MathExtensions::Limit(StringExtensions::ToInteger(part), 1, INT_MAX);
               if (m_durationMs > 0 && m_blink >= 1)
               {
                  m_blinkIntervalMs = MathExtensions::Limit(m_durationMs / m_blink, 1, INT_MAX);
                  m_durationMs = 0;
               }
            }
            break;
         default:
            Log::Warning(StringExtensions::Build("The ledcontrol table config setting {0} contains more than 2 numeric values without a type definition.", settingData));
            throw std::runtime_error(StringExtensions::Build("The ledcontrol table config setting {0} contains more than 2 numeric values without a type definition.", settingData));
         }
         integerCnt++;
      }
      else if (partNr == 0)
      {
         m_colorName = part;
      }
      else
      {
         Log::Warning(StringExtensions::Build("Cant parse the part {0} of the ledcontrol table config setting {1}", part, settingData));
         throw std::runtime_error(StringExtensions::Build("Cant parse the part {0} of the ledcontrol table config setting {1}", part, settingData));
      }

      partNr++;
   }
}

OutputTypeEnum TableConfigSetting::GetOutputType() const
{
   if (!StringExtensions::IsNullOrWhiteSpace(m_colorName))
      return OutputTypeEnum::RGBOutput;
   return OutputTypeEnum::AnalogOutput;
}

int TableConfigSetting::GetIntensity() const
{
   if (!StringExtensions::IsNullOrWhiteSpace(m_colorName))
      return -1;
   return m_intensity;
}

void TableConfigSetting::SetBlinkPulseWidth(int width) { m_blinkPulseWidth = MathExtensions::Limit(width, 1, 99); }
void TableConfigSetting::SetBlinkPulseWidthNested(int width) { m_blinkPulseWidthNested = MathExtensions::Limit(width, 1, 99); }

}