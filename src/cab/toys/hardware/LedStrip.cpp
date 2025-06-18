#include "LedStrip.h"
#include "../../out/ISupportsSetValues.h"
#include "../../out/IOutputController.h"
#include "../../out/OutputControllerList.h"
#include "../../out/Output.h"
#include "../../Cabinet.h"
#include "../../overrides/TableOverrideSettings.h"
#include "../../schedules/ScheduledSettings.h"
#include "../layer/AlphaMappingTable.h"
#include "../lwequivalent/LedWizEquivalent.h"
#include "../ToyList.h"
#include "../../../general/MathExtensions.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include <tinyxml2/tinyxml2.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <vector>

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

LedStrip::~LedStrip()
{
   if (m_fadingCurve)
   {
      delete m_fadingCurve;
      m_fadingCurve = nullptr;
   }
}

void LedStrip::Init(Cabinet* cabinet)
{
   m_cabinet = cabinet;

   m_layers.SetWidth(m_width);
   m_layers.SetHeight(m_height);

   m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear);
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
      if (cabinet->GetOutputControllers()->Contains(m_outputControllerName))
      {
         IOutputController* controller = cabinet->GetOutputControllers()->GetByName(m_outputControllerName);
         m_outputController = dynamic_cast<ISupportsSetValues*>(controller);
      }
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
   if (m_fadingCurve)
   {
      delete m_fadingCurve;
      m_fadingCurve = nullptr;
   }
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
   m_outputMappingTable.resize(m_width, std::vector<int>(m_height, 0));

   for (int y = 0; y < m_height; y++)
   {
      for (int x = 0; x < m_width; x++)
      {
         int ledNumber = CalculateLedNumber(x, y);
         m_outputMappingTable[x][y] = ledNumber * 3;
      }
   }
}

void LedStrip::SetOutputData()
{
   if (!m_layers.empty())
   {
      if (m_brightness == 0.0f)
      {
         std::fill(m_outputData.begin(), m_outputData.end(), 0);
         return;
      }

      std::array<float, 3> defaultValue = { { 0.0f, 0.0f, 0.0f } };
      std::vector<std::vector<std::array<float, 3>>> value(m_width, std::vector<std::array<float, 3>>(m_height, defaultValue));

      for (const auto& [layerNr, data] : m_layers)
      {
         if (!data)
            continue;

         for (int y = 0; y < m_height; y++)
         {
            for (int x = 0; x < m_width; x++)
            {
               int idx = y * m_width + x;
               const RGBAColor& d = data[idx];

               int alpha = MathExtensions::Limit(d.GetAlpha(), 0, 255);
               if (alpha != 0)
               {
                  value[x][y][0]
                     = AlphaMappingTable::AlphaMapping[255 - alpha][static_cast<int>(value[x][y][0])] + AlphaMappingTable::AlphaMapping[alpha][MathExtensions::Limit(d.GetRed(), 0, 255)];
                  value[x][y][1]
                     = AlphaMappingTable::AlphaMapping[255 - alpha][static_cast<int>(value[x][y][1])] + AlphaMappingTable::AlphaMapping[alpha][MathExtensions::Limit(d.GetGreen(), 0, 255)];
                  value[x][y][2]
                     = AlphaMappingTable::AlphaMapping[255 - alpha][static_cast<int>(value[x][y][2])] + AlphaMappingTable::AlphaMapping[alpha][MathExtensions::Limit(d.GetBlue(), 0, 255)];
               }
            }
         }
      }

      const uint8_t* fadingTable = m_fadingCurve->GetData();

      Output newOutput;

      LedWizEquivalent* lwe = nullptr;
      if (m_cabinet && m_cabinet->GetToys())
      {
         ToyList* toys = m_cabinet->GetToys();
         for (auto it = toys->begin(); it != toys->end(); ++it)
         {
            lwe = dynamic_cast<LedWizEquivalent*>(*it);
            if (lwe)
               break;
         }
      }

      newOutput.SetNumber(m_firstLedNumber);
      newOutput.SetOutput(100);

      if (lwe)
      {
         IOutput* overriddenOutput = TableOverrideSettings::GetInstance()->GetNewRecalculatedOutput(&newOutput, 30, lwe->GetLedWizNumber() - 30);
         newOutput.SetOutput(overriddenOutput->GetOutput());
         if (overriddenOutput != &newOutput)
            delete overriddenOutput;

         IOutput* scheduledOutput = ScheduledSettings::GetInstance().GetNewRecalculatedOutput(&newOutput, 30, lwe->GetLedWizNumber() - 30);
         newOutput.SetOutput(scheduledOutput->GetOutput());
         if (scheduledOutput != &newOutput)
            delete scheduledOutput;
      }

      Curve finalFadingTable = GetFadingTableFromPercent(newOutput.GetOutput());
      fadingTable = finalFadingTable.GetData();

      switch (m_colorOrder)
      {
      case RGBOrderEnum::RBG:
         for (int y = 0; y < m_height; y++)
         {
            for (int x = 0; x < m_width; x++)
            {
               int outputNumber = m_outputMappingTable[x][y];
               m_outputData[outputNumber] = fadingTable[static_cast<int>(value[x][y][0])];
               m_outputData[outputNumber + 2] = fadingTable[static_cast<int>(value[x][y][1])];
               m_outputData[outputNumber + 1] = fadingTable[static_cast<int>(value[x][y][2])];
            }
         }
         break;
      case RGBOrderEnum::GRB:
         for (int y = 0; y < m_height; y++)
         {
            for (int x = 0; x < m_width; x++)
            {
               int outputNumber = m_outputMappingTable[x][y];
               m_outputData[outputNumber + 1] = fadingTable[static_cast<int>(value[x][y][0])];
               m_outputData[outputNumber] = fadingTable[static_cast<int>(value[x][y][1])];
               m_outputData[outputNumber + 2] = fadingTable[static_cast<int>(value[x][y][2])];
            }
         }
         break;
      case RGBOrderEnum::GBR:
         for (int y = 0; y < m_height; y++)
         {
            for (int x = 0; x < m_width; x++)
            {
               int outputNumber = m_outputMappingTable[x][y];
               m_outputData[outputNumber + 1] = fadingTable[static_cast<int>(value[x][y][0])];
               m_outputData[outputNumber + 2] = fadingTable[static_cast<int>(value[x][y][1])];
               m_outputData[outputNumber] = fadingTable[static_cast<int>(value[x][y][2])];
            }
         }
         break;
      case RGBOrderEnum::BRG:
         for (int y = 0; y < m_height; y++)
         {
            for (int x = 0; x < m_width; x++)
            {
               int outputNumber = m_outputMappingTable[x][y];
               m_outputData[outputNumber + 2] = fadingTable[static_cast<int>(value[x][y][0])];
               m_outputData[outputNumber] = fadingTable[static_cast<int>(value[x][y][1])];
               m_outputData[outputNumber + 1] = fadingTable[static_cast<int>(value[x][y][2])];
            }
         }
         break;
      case RGBOrderEnum::BGR:
         for (int y = 0; y < m_height; y++)
         {
            for (int x = 0; x < m_width; x++)
            {
               int outputNumber = m_outputMappingTable[x][y];
               m_outputData[outputNumber + 2] = fadingTable[static_cast<int>(value[x][y][0])];
               m_outputData[outputNumber + 1] = fadingTable[static_cast<int>(value[x][y][1])];
               m_outputData[outputNumber] = fadingTable[static_cast<int>(value[x][y][2])];
            }
         }
         break;
      case RGBOrderEnum::RGB:
      default:
         for (int y = 0; y < m_height; y++)
         {
            for (int x = 0; x < m_width; x++)
            {
               int outputNumber = m_outputMappingTable[x][y];
               m_outputData[outputNumber] = fadingTable[static_cast<int>(value[x][y][0])];
               m_outputData[outputNumber + 1] = fadingTable[static_cast<int>(value[x][y][1])];
               m_outputData[outputNumber + 2] = fadingTable[static_cast<int>(value[x][y][2])];
            }
         }
         break;
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


uint8_t LedStrip::ApplyFadingCurve(uint8_t value) const
{
   if (m_fadingCurve != nullptr)
   {
      return m_fadingCurve->MapValue(value);
   }
   return value;
}

Curve LedStrip::GetFadingTableFromPercent(int outputPercent) const
{
   if (outputPercent == 100)
   {
      return *m_fadingCurve;
   }
   else if (outputPercent >= 89)
   {
      return Curve(Curve::CurveTypeEnum::Linear0To224);
   }
   else if (outputPercent >= 78)
   {
      return Curve(Curve::CurveTypeEnum::Linear0To192);
   }
   else if (outputPercent >= 67)
   {
      return Curve(Curve::CurveTypeEnum::Linear0To160);
   }
   else if (outputPercent >= 56)
   {
      return Curve(Curve::CurveTypeEnum::Linear0To128);
   }
   else if (outputPercent >= 45)
   {
      return Curve(Curve::CurveTypeEnum::Linear0To96);
   }
   else if (outputPercent >= 34)
   {
      return Curve(Curve::CurveTypeEnum::Linear0To64);
   }
   else if (outputPercent >= 23)
   {
      return Curve(Curve::CurveTypeEnum::Linear0To32);
   }
   else
   {
      return Curve(Curve::CurveTypeEnum::Linear0To16);
   }
}


bool LedStrip::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   auto loadElement = [&](const char* name, auto setter)
   {
      const tinyxml2::XMLElement* elem = element->FirstChildElement(name);
      if (elem && elem->GetText())
      {
         try
         {
            setter(elem->GetText());
         }
         catch (...)
         {
            return false;
         }
      }
      return true;
   };

   loadElement("Name", [this](const char* text) { SetName(text); });
   loadElement("Width", [this](const char* text) { SetWidth(std::stoi(text)); });
   loadElement("Height", [this](const char* text) { SetHeight(std::stoi(text)); });
   loadElement("FirstLedNumber", [this](const char* text) { SetFirstLedNumber(std::stoi(text)); });
   loadElement("OutputControllerName", [this](const char* text) { SetOutputControllerName(text); });
   loadElement("FadingCurveName", [this](const char* text) { SetFadingCurveName(text); });
   loadElement("Brightness", [this](const char* text) { SetBrightness(std::stoi(text)); });
   loadElement("BrightnessGammaCorrection", [this](const char* text) { SetBrightnessGammaCorrection(std::stof(text)); });

   loadElement("LedStripArrangement",
      [this](const char* text)
      {
         std::string arrangement = text;
         if (arrangement == "LeftRightTopDown")
            SetLedStripArrangement(LedStripArrangementEnum::LeftRightTopDown);
         else if (arrangement == "LeftRightBottomUp")
            SetLedStripArrangement(LedStripArrangementEnum::LeftRightBottomUp);
         else if (arrangement == "RightLeftTopDown")
            SetLedStripArrangement(LedStripArrangementEnum::RightLeftTopDown);
         else if (arrangement == "RightLeftBottomUp")
            SetLedStripArrangement(LedStripArrangementEnum::RightLeftBottomUp);
         else if (arrangement == "TopDownLeftRight")
            SetLedStripArrangement(LedStripArrangementEnum::TopDownLeftRight);
         else if (arrangement == "TopDownRightLeft")
            SetLedStripArrangement(LedStripArrangementEnum::TopDownRightLeft);
         else if (arrangement == "BottomUpLeftRight")
            SetLedStripArrangement(LedStripArrangementEnum::BottomUpLeftRight);
         else if (arrangement == "BottomUpRightLeft")
            SetLedStripArrangement(LedStripArrangementEnum::BottomUpRightLeft);
      });

   loadElement("ColorOrder",
      [this](const char* text)
      {
         std::string colorOrder = text;
         if (colorOrder == "RGB")
            SetColorOrder(RGBOrderEnum::RGB);
         else if (colorOrder == "RBG")
            SetColorOrder(RGBOrderEnum::RBG);
         else if (colorOrder == "GRB")
            SetColorOrder(RGBOrderEnum::GRB);
         else if (colorOrder == "GBR")
            SetColorOrder(RGBOrderEnum::GBR);
         else if (colorOrder == "BRG")
            SetColorOrder(RGBOrderEnum::BRG);
         else if (colorOrder == "BGR")
            SetColorOrder(RGBOrderEnum::BGR);
      });

   return true;
}

}
