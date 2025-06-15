#include "LedStrip.h"
#include "../../out/ISupportsSetValues.h"
#include "../../Cabinet.h"
#include "../layer/AlphaMappingTable.h"
#include "../../../general/MathExtensions.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace DOF
{

LedStrip::LedStrip()
   : m_width(1)
   , m_height(1)
   , m_ledStripArrangement(LedStripArrangementEnum::LeftRightTopDown)
   , m_colorOrder(RGBOrderEnum::RBG)
   , m_firstLedNumber(1)
   , m_brightness(1.0f)
   , m_brightnessGammaCorrection(2.2f)
   , m_outputControllerName("")
   , m_fadingCurveName("Linear")
   , m_layers(1, 1)
   , m_fadingCurve(nullptr)
   , m_outputController(nullptr)
   , m_cabinet(nullptr)
{
}

LedStrip::~LedStrip() { }

void LedStrip::Init(Cabinet* cabinet)
{
   m_cabinet = cabinet;

   m_layers.SetWidth(m_width);
   m_layers.SetHeight(m_height);

   m_fadingCurve = std::make_unique<Curve>(Curve::CurveTypeEnum::Linear);
   if (!m_fadingCurveName.empty() && m_fadingCurveName != "Linear")
   {
      if (m_fadingCurveName == "SwissLizardsLedCurve")
      {
         m_fadingCurve->SetCurve(Curve::CurveTypeEnum::SwissLizardsLedCurve);
      }
      else if (m_fadingCurveName == "InvertedLinear")
      {
         m_fadingCurve->SetCurve(Curve::CurveTypeEnum::InvertedLinear);
      }
   }

   if (!m_outputControllerName.empty() && cabinet != nullptr)
   {
      m_outputController = nullptr;
   }

   int outputCount = GetNumberOfOutputs();
   m_outputData.resize(outputCount, 0);

   BuildMappingTables();
}

void LedStrip::Reset()
{
   m_layers.Clear();
   std::fill(m_outputData.begin(), m_outputData.end(), 0);
   UpdateOutputs();
}

void LedStrip::Finish()
{
   Reset();
   m_outputController = nullptr;
}

void LedStrip::UpdateToy() { UpdateOutputs(); }

RGBAColor* LedStrip::GetLayer(int layerNr) { return m_layers.GetOrCreateLayer(layerNr); }

RGBAColor LedStrip::GetElement(int layerNr, int x, int y) { return m_layers.GetElement(layerNr, x, y); }

void LedStrip::SetElement(int layerNr, int x, int y, const RGBAColor& value) { m_layers.SetElement(layerNr, x, y, value); }

void LedStrip::SetWidth(int value)
{
   int newWidth = MathExtensions::Limit(value, 1, 1000);
   if (newWidth != m_width)
   {
      m_width = newWidth;
      m_layers.SetWidth(m_width);
      m_outputData.resize(GetNumberOfOutputs(), 0);
      BuildMappingTables();
   }
}

void LedStrip::SetHeight(int value)
{
   int newHeight = MathExtensions::Limit(value, 1, 1000);
   if (newHeight != m_height)
   {
      m_height = newHeight;
      m_layers.SetHeight(m_height);
      m_outputData.resize(GetNumberOfOutputs(), 0);
      BuildMappingTables();
   }
}

void LedStrip::SetFirstLedNumber(int value) { m_firstLedNumber = MathExtensions::Limit(value, 1, 10000); }

void LedStrip::SetBrightness(int value) { m_brightness = MathExtensions::Limit(static_cast<float>(value) / 100.0f, 0.0f, 1.0f); }

void LedStrip::SetFadingCurveName(const std::string& name)
{
   m_fadingCurveName = name;

   if (m_fadingCurve != nullptr)
   {
      if (name == "SwissLizardsLedCurve")
      {
         m_fadingCurve->SetCurve(Curve::CurveTypeEnum::SwissLizardsLedCurve);
      }
      else if (name == "InvertedLinear")
      {
         m_fadingCurve->SetCurve(Curve::CurveTypeEnum::InvertedLinear);
      }
      else
      {
         m_fadingCurve->SetCurve(Curve::CurveTypeEnum::Linear);
      }
   }
}

void LedStrip::SetBrightnessGammaCorrection(float value) { m_brightnessGammaCorrection = MathExtensions::Limit(value, 0.1f, 10.0f); }

void LedStrip::UpdateOutputs()
{
   SetOutputData();

   if (m_outputController != nullptr && !m_outputData.empty())
   {
      int firstOutput = (m_firstLedNumber - 1) * 3;
      m_outputController->SetValues(firstOutput, m_outputData.data(), static_cast<int>(m_outputData.size()));
   }
}

void LedStrip::BuildMappingTables()
{
   m_outputMappingTable.clear();
   m_outputMappingTable.resize(m_height, std::vector<int>(m_width, 0));

   for (int y = 0; y < m_height; y++)
   {
      for (int x = 0; x < m_width; x++)
      {
         int ledNumber = CalculateLedNumber(x, y);
         m_outputMappingTable[y][x] = ledNumber;
      }
   }
}

void LedStrip::SetOutputData()
{
   for (int y = 0; y < m_height; y++)
   {
      for (int x = 0; x < m_width; x++)
      {
         RGBAColor blendedColor = BlendLayers(x, y);

         float brightnessMultiplier = ApplyGammaCorrection(m_brightness);

         uint8_t red = static_cast<uint8_t>(MathExtensions::Limit(blendedColor.GetRed() * brightnessMultiplier, 0.0f, 255.0f));
         uint8_t green = static_cast<uint8_t>(MathExtensions::Limit(blendedColor.GetGreen() * brightnessMultiplier, 0.0f, 255.0f));
         uint8_t blue = static_cast<uint8_t>(MathExtensions::Limit(blendedColor.GetBlue() * brightnessMultiplier, 0.0f, 255.0f));

         red = ApplyFadingCurve(red);
         green = ApplyFadingCurve(green);
         blue = ApplyFadingCurve(blue);

         ApplyColorOrder(red, green, blue);

         int ledPosition = m_outputMappingTable[y][x];
         int baseOutputIndex = ledPosition * 3;

         if (baseOutputIndex >= 0 && baseOutputIndex + 2 < static_cast<int>(m_outputData.size()))
         {
            m_outputData[baseOutputIndex] = red;
            m_outputData[baseOutputIndex + 1] = green;
            m_outputData[baseOutputIndex + 2] = blue;
         }
      }
   }
}

int LedStrip::CalculateLedNumber(int x, int y) const
{
   int ledNumber = 0;

   switch (m_ledStripArrangement)
   {
   case LedStripArrangementEnum::LeftRightTopDown: ledNumber = (y * m_width) + x; break;

   case LedStripArrangementEnum::LeftRightBottomUp: ledNumber = ((m_height - 1 - y) * m_width) + x; break;

   case LedStripArrangementEnum::RightLeftTopDown: ledNumber = (y * m_width) + (m_width - 1 - x); break;

   case LedStripArrangementEnum::RightLeftBottomUp: ledNumber = ((m_height - 1 - y) * m_width) + (m_width - 1 - x); break;

   case LedStripArrangementEnum::TopDownLeftRight: ledNumber = (x * m_height) + y; break;

   case LedStripArrangementEnum::TopDownRightLeft: ledNumber = ((m_width - 1 - x) * m_height) + y; break;

   case LedStripArrangementEnum::BottomUpLeftRight: ledNumber = (x * m_height) + (m_height - 1 - y); break;

   case LedStripArrangementEnum::BottomUpRightLeft: ledNumber = ((m_width - 1 - x) * m_height) + (m_height - 1 - y); break;

   case LedStripArrangementEnum::LeftRightAlternateTopDown:
      if ((y & 1) == 0)
         ledNumber = (m_width * y) + x;
      else
         ledNumber = (m_width * y) + (m_width - 1 - x);
      break;

   case LedStripArrangementEnum::LeftRightAlternateBottomUp:
   {
      int actualY = m_height - 1 - y;
      if ((actualY & 1) == 0)
         ledNumber = (m_width * actualY) + x;
      else
         ledNumber = (m_width * actualY) + (m_width - 1 - x);
   }
   break;

   case LedStripArrangementEnum::RightLeftAlternateTopDown:
      if ((y & 1) == 0)
         ledNumber = (m_width * y) + (m_width - 1 - x);
      else
         ledNumber = (m_width * y) + x;
      break;

   case LedStripArrangementEnum::RightLeftAlternateBottomUp:
   {
      int actualY = m_height - 1 - y;
      if ((actualY & 1) == 0)
         ledNumber = (m_width * actualY) + (m_width - 1 - x);
      else
         ledNumber = (m_width * actualY) + x;
   }
   break;

   case LedStripArrangementEnum::TopDownAlternateLeftRight:
      if ((x & 1) == 0)
         ledNumber = (m_height * x) + y;
      else
         ledNumber = (m_height * x) + (m_height - 1 - y);
      break;

   case LedStripArrangementEnum::TopDownAlternateRightLeft:
   {
      int actualX = m_width - 1 - x;
      if ((actualX & 1) == 0)
         ledNumber = (m_height * actualX) + y;
      else
         ledNumber = (m_height * actualX) + (m_height - 1 - y);
   }
   break;

   case LedStripArrangementEnum::BottomUpAlternateLeftRight:
      if ((x & 1) == 0)
         ledNumber = (m_height * x) + (m_height - 1 - y);
      else
         ledNumber = (m_height * x) + y;
      break;

   case LedStripArrangementEnum::BottomUpAlternateRightLeft:
   {
      int actualX = m_width - 1 - x;
      if ((actualX & 1) == 0)
         ledNumber = (m_height * actualX) + (m_height - 1 - y);
      else
         ledNumber = (m_height * actualX) + y;
   }
   break;
   }

   return ledNumber;
}

void LedStrip::ApplyColorOrder(uint8_t& r, uint8_t& g, uint8_t& b) const
{
   uint8_t originalR = r, originalG = g, originalB = b;

   switch (m_colorOrder)
   {
   case RGBOrderEnum::RGB: break;
   case RGBOrderEnum::RBG:
      g = originalB;
      b = originalG;
      break;
   case RGBOrderEnum::GRB:
      r = originalG;
      g = originalR;
      break;
   case RGBOrderEnum::GBR:
      r = originalG;
      g = originalB;
      b = originalR;
      break;
   case RGBOrderEnum::BRG:
      r = originalB;
      g = originalR;
      b = originalG;
      break;
   case RGBOrderEnum::BGR:
      r = originalB;
      b = originalR;
      break;
   }
}

RGBAColor LedStrip::BlendLayers(int x, int y) const
{
   RGBAColor result(0, 0, 0, 0);

   for (const auto& pair : m_layers)
   {
      int layerNr = pair.first;
      RGBAColor* layerData = pair.second;

      if (layerData != nullptr)
      {
         int index = y * m_width + x;
         if (index >= 0 && index < m_width * m_height)
         {
            const RGBAColor& layerColor = layerData[index];

            if (layerColor.GetAlpha() > 0)
            {
               float alpha = static_cast<float>(layerColor.GetAlpha()) / 255.0f;
               float invAlpha = 1.0f - alpha;

               int newRed = static_cast<int>(result.GetRed() * invAlpha + layerColor.GetRed() * alpha);
               int newGreen = static_cast<int>(result.GetGreen() * invAlpha + layerColor.GetGreen() * alpha);
               int newBlue = static_cast<int>(result.GetBlue() * invAlpha + layerColor.GetBlue() * alpha);
               int newAlpha = std::max(result.GetAlpha(), layerColor.GetAlpha());

               result.SetRGBA(
                  MathExtensions::Limit(newRed, 0, 255), MathExtensions::Limit(newGreen, 0, 255), MathExtensions::Limit(newBlue, 0, 255), MathExtensions::Limit(newAlpha, 0, 255));
            }
         }
      }
   }

   return result;
}

uint8_t LedStrip::ApplyFadingCurve(uint8_t value) const
{
   if (m_fadingCurve != nullptr)
   {
      return m_fadingCurve->MapValue(value);
   }
   return value;
}

float LedStrip::ApplyGammaCorrection(float value) const { return std::pow(value, 1.0f / m_brightnessGammaCorrection); }


}