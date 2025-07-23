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
#include "../../../general/CurveList.h"
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
   , m_fadingCurveName("Linear")
   , m_fadingCurve(nullptr)
   , m_brightnessGammaCorrection(2.2f)
   , m_brightness(1.0f)
   , m_outputControllerName("")
   , m_outputController(nullptr)
   , m_layers()
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

void LedStrip::SetWidth(int value)
{
   int newWidth = MathExtensions::Limit(value, 1, 1000);
   if (newWidth != m_width)
   {
      m_width = newWidth;
      m_layers.SetWidth(m_width);
   }
}

void LedStrip::SetHeight(int value)
{
   int newHeight = MathExtensions::Limit(value, 1, 1000);
   if (newHeight != m_height)
   {
      m_height = newHeight;
      m_layers.SetHeight(m_height);
   }
}

void LedStrip::SetFirstLedNumber(int value) { m_firstLedNumber = MathExtensions::Limit(value, 1, 10000); }

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

void LedStrip::SetBrightness(int value) { m_brightness = MathExtensions::Limit(static_cast<float>(value) / 100.0f, 0.0f, 1.0f); }

void LedStrip::InitFadingCurve(Cabinet* cabinet)
{
   if (cabinet && cabinet->GetCurves() && cabinet->GetCurves()->Contains(m_fadingCurveName))
   {
      m_fadingCurve = cabinet->GetCurves()->GetByName(m_fadingCurveName);
   }
   else if (!m_fadingCurveName.empty() && m_fadingCurveName != "Linear")
   {
      if (m_fadingCurveName == "SwissLizardsLedCurve")
      {
         m_fadingCurve = new Curve(Curve::CurveTypeEnum::SwissLizardsLedCurve);
      }
      else if (m_fadingCurveName == "InvertedLinear")
      {
         m_fadingCurve = new Curve(Curve::CurveTypeEnum::InvertedLinear);
      }
      else
      {
         if (cabinet && cabinet->GetCurves())
         {
            m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear);
            m_fadingCurve->SetName(m_fadingCurveName);
            cabinet->GetCurves()->Add(m_fadingCurveName, m_fadingCurve);
            m_fadingCurve = cabinet->GetCurves()->GetByName(m_fadingCurveName);
         }
         else
         {
            m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear);
            m_fadingCurve->SetName(m_fadingCurveName);
         }
      }
   }
   else
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear);
   }
}

void LedStrip::Init(Cabinet* cabinet)
{
   m_cabinet = cabinet;

   if (!m_outputControllerName.empty() && cabinet != nullptr)
   {
      if (cabinet->GetOutputControllers()->Contains(m_outputControllerName))
      {
         IOutputController* controller = cabinet->GetOutputControllers()->GetByName(m_outputControllerName);
         m_outputController = dynamic_cast<ISupportsSetValues*>(controller);
      }
   }

   BuildMappingTables();
   m_outputData.resize(GetNumberOfOutputs(), 0);
   InitFadingCurve(cabinet);

   m_layers = MatrixDictionaryBase<RGBAColor>();
   m_layers.SetWidth(m_width);
   m_layers.SetHeight(m_height);
}

void LedStrip::Reset()
{
   if (m_outputController != nullptr && GetNumberOfLeds() > 0)
   {
      std::vector<uint8_t> clearData(GetNumberOfLeds() * 3, 0);
      m_outputController->SetValues((m_firstLedNumber - 1) * 3, clearData.data(), static_cast<int>(clearData.size()));
   }
}

void LedStrip::Finish()
{
   Reset();
   m_outputController = nullptr;
   if (m_fadingCurve && m_cabinet && m_cabinet->GetCurves() && m_cabinet->GetCurves()->Contains(m_fadingCurveName))
   {
      m_fadingCurve = nullptr;
   }
   else if (m_fadingCurve)
   {
      delete m_fadingCurve;
      m_fadingCurve = nullptr;
   }
}

void LedStrip::UpdateToy() { UpdateOutputs(); }

void LedStrip::UpdateOutputs()
{
   if (m_outputController != nullptr && m_layers.size() > 0)
   {
      SetOutputData();
      m_outputController->SetValues((m_firstLedNumber - 1) * 3, m_outputData.data(), static_cast<int>(m_outputData.size()));
   }
}

RGBAColor* LedStrip::GetLayer(int layerNr) { return m_layers.GetOrCreateLayer(layerNr); }

RGBAColor LedStrip::GetElement(int layerNr, int x, int y) { return m_layers.GetElement(layerNr, x, y); }

void LedStrip::SetElement(int layerNr, int x, int y, const RGBAColor& value) { m_layers.SetElement(layerNr, x, y, value); }

void LedStrip::BuildMappingTables()
{
   m_outputMappingTable.resize(m_width, std::vector<int>(m_height, 0));
   int ledNr = 0;

   for (int y = 0; y < m_height; y++)
   {
      for (int x = 0; x < m_width; x++)
      {
         switch (m_ledStripArrangement)
         {
         case LedStripArrangementEnum::LeftRightTopDown: ledNr = (y * m_width) + x; break;
         case LedStripArrangementEnum::LeftRightBottomUp: ledNr = ((m_height - 1 - y) * m_width) + x; break;
         case LedStripArrangementEnum::RightLeftTopDown: ledNr = (y * m_width) + (m_width - 1 - x); break;
         case LedStripArrangementEnum::RightLeftBottomUp: ledNr = ((m_height - 1 - y) * m_width) + (m_width - 1 - x); break;
         case LedStripArrangementEnum::TopDownLeftRight: ledNr = x * m_height + y; break;
         case LedStripArrangementEnum::TopDownRightLeft: ledNr = ((m_width - 1 - x) * m_height) + y; break;
         case LedStripArrangementEnum::BottomUpLeftRight: ledNr = (x * m_height) + (m_height - 1 - y); break;
         case LedStripArrangementEnum::BottomUpRightLeft: ledNr = ((m_width - 1 - x) * m_height) + (m_height - 1 - y); break;
         case LedStripArrangementEnum::LeftRightAlternateTopDown: ledNr = (m_width * y) + ((y & 1) == 0 ? x : (m_width - 1 - x)); break;
         case LedStripArrangementEnum::LeftRightAlternateBottomUp: ledNr = (m_width * (m_height - 1 - y)) + (((m_height - 1 - y) & 1) == 0 ? x : (m_width - 1 - x)); break;
         case LedStripArrangementEnum::RightLeftAlternateTopDown: ledNr = (m_width * y) + ((y & 1) == 1 ? x : (m_width - 1 - x)); break;
         case LedStripArrangementEnum::RightLeftAlternateBottomUp: ledNr = (m_width * (m_height - 1 - y)) + (((m_height - 1 - y) & 1) == 1 ? x : (m_width - 1 - x)); break;
         case LedStripArrangementEnum::TopDownAlternateLeftRight: ledNr = (m_height * x) + ((x & 1) == 0 ? y : (m_height - 1 - y)); break;
         case LedStripArrangementEnum::TopDownAlternateRightLeft: ledNr = (m_height * (m_width - 1 - x)) + ((x & 1) == 1 ? y : (m_height - 1 - y)); break;
         case LedStripArrangementEnum::BottomUpAlternateLeftRight: ledNr = (m_height * x) + ((x & 1) == 1 ? y : (m_height - 1 - y)); break;
         case LedStripArrangementEnum::BottomUpAlternateRightLeft: ledNr = (m_height * (m_width - 1 - x)) + ((x & 1) == 0 ? y : (m_height - 1 - y)); break;
         default: ledNr = (y * m_width) + x; break;
         }
         m_outputMappingTable[x][y] = ledNr * 3;
      }
   }
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

void LedStrip::SetOutputData()
{
   if (m_layers.size() > 0)
   {
      if (m_brightness == 0.0f)
      {
         std::fill(m_outputData.begin(), m_outputData.end(), 0);
         return;
      }

      std::array<float, 3> defaultValue = { { 0.0f, 0.0f, 0.0f } };
      std::vector<std::vector<std::array<float, 3>>> value(m_width, std::vector<std::array<float, 3>>(m_height, defaultValue));

      for (const auto& kv : m_layers)
      {
         RGBAColor* d = kv.second;

         int nr = 0;
         for (int y = 0; y < m_height; y++)
         {
            for (int x = 0; x < m_width; x++)
            {
               int alpha = MathExtensions::Limit(d[nr].GetAlpha(), 0, 255);
               if (alpha != 0)
               {
                  value[x][y][0]
                     = AlphaMappingTable::AlphaMapping[255 - alpha][static_cast<int>(value[x][y][0])] + AlphaMappingTable::AlphaMapping[alpha][MathExtensions::Limit(d[nr].GetRed(), 0, 255)];
                  value[x][y][1] = AlphaMappingTable::AlphaMapping[255 - alpha][static_cast<int>(value[x][y][1])]
                     + AlphaMappingTable::AlphaMapping[alpha][MathExtensions::Limit(d[nr].GetGreen(), 0, 255)];
                  value[x][y][2] = AlphaMappingTable::AlphaMapping[255 - alpha][static_cast<int>(value[x][y][2])]
                     + AlphaMappingTable::AlphaMapping[alpha][MathExtensions::Limit(d[nr].GetBlue(), 0, 255)];
               }
               nr++;
            }
         }
      }

      const uint8_t* fadingTable = m_fadingCurve->GetData();

      Output newOutput;
      std::vector<LedWizEquivalent*> lweList;
      if (m_cabinet && m_cabinet->GetToys())
      {
         ToyList* toys = m_cabinet->GetToys();
         for (auto it = toys->begin(); it != toys->end(); ++it)
         {
            LedWizEquivalent* lwe = dynamic_cast<LedWizEquivalent*>(*it);
            if (lwe)
            {
               lweList.push_back(lwe);
            }
         }
      }
      LedWizEquivalent* lwe = lweList.empty() ? nullptr : lweList[0];

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

      if (m_brightness < 1.0f)
      {
         float correctedBrightness = std::pow(m_brightness, m_brightnessGammaCorrection);
         for (int num = 0; num < static_cast<int>(m_outputData.size()); ++num)
         {
            m_outputData[num] = static_cast<uint8_t>(m_outputData[num] * correctedBrightness);
         }
      }
   }
}

tinyxml2::XMLElement* LedStrip::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   if (!GetName().empty())
      element->SetAttribute("Name", GetName().c_str());

   auto addElement = [&](const char* name, const std::string& value)
   {
      tinyxml2::XMLElement* elem = doc.NewElement(name);
      elem->SetText(value.c_str());
      element->InsertEndChild(elem);
   };

   addElement("Width", std::to_string(m_width));
   addElement("Height", std::to_string(m_height));
   addElement("FirstLedNumber", std::to_string(m_firstLedNumber));
   addElement("OutputControllerName", m_outputControllerName);
   addElement("FadingCurveName", m_fadingCurveName);
   addElement("Brightness", std::to_string(static_cast<int>(m_brightness * 100)));
   addElement("BrightnessGammaCorrection", std::to_string(m_brightnessGammaCorrection));

   std::string arrangementStr;
   switch (m_ledStripArrangement)
   {
   case LedStripArrangementEnum::LeftRightTopDown: arrangementStr = "LeftRightTopDown"; break;
   case LedStripArrangementEnum::LeftRightBottomUp: arrangementStr = "LeftRightBottomUp"; break;
   case LedStripArrangementEnum::RightLeftTopDown: arrangementStr = "RightLeftTopDown"; break;
   case LedStripArrangementEnum::RightLeftBottomUp: arrangementStr = "RightLeftBottomUp"; break;
   case LedStripArrangementEnum::TopDownLeftRight: arrangementStr = "TopDownLeftRight"; break;
   case LedStripArrangementEnum::TopDownRightLeft: arrangementStr = "TopDownRightLeft"; break;
   case LedStripArrangementEnum::BottomUpLeftRight: arrangementStr = "BottomUpLeftRight"; break;
   case LedStripArrangementEnum::BottomUpRightLeft: arrangementStr = "BottomUpRightLeft"; break;
   case LedStripArrangementEnum::LeftRightAlternateTopDown: arrangementStr = "LeftRightAlternateTopDown"; break;
   case LedStripArrangementEnum::LeftRightAlternateBottomUp: arrangementStr = "LeftRightAlternateBottomUp"; break;
   case LedStripArrangementEnum::RightLeftAlternateTopDown: arrangementStr = "RightLeftAlternateTopDown"; break;
   case LedStripArrangementEnum::RightLeftAlternateBottomUp: arrangementStr = "RightLeftAlternateBottomUp"; break;
   case LedStripArrangementEnum::TopDownAlternateLeftRight: arrangementStr = "TopDownAlternateLeftRight"; break;
   case LedStripArrangementEnum::TopDownAlternateRightLeft: arrangementStr = "TopDownAlternateRightLeft"; break;
   case LedStripArrangementEnum::BottomUpAlternateLeftRight: arrangementStr = "BottomUpAlternateLeftRight"; break;
   case LedStripArrangementEnum::BottomUpAlternateRightLeft: arrangementStr = "BottomUpAlternateRightLeft"; break;
   default: arrangementStr = "LeftRightTopDown"; break;
   }
   addElement("LedStripArrangement", arrangementStr);

   std::string colorOrderStr;
   switch (m_colorOrder)
   {
   case RGBOrderEnum::RGB: colorOrderStr = "RGB"; break;
   case RGBOrderEnum::RBG: colorOrderStr = "RBG"; break;
   case RGBOrderEnum::GRB: colorOrderStr = "GRB"; break;
   case RGBOrderEnum::GBR: colorOrderStr = "GBR"; break;
   case RGBOrderEnum::BRG: colorOrderStr = "BRG"; break;
   case RGBOrderEnum::BGR: colorOrderStr = "BGR"; break;
   default: colorOrderStr = "RBG"; break;
   }
   addElement("ColorOrder", colorOrderStr);

   return element;
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
         else if (arrangement == "LeftRightAlternateTopDown")
            SetLedStripArrangement(LedStripArrangementEnum::LeftRightAlternateTopDown);
         else if (arrangement == "LeftRightAlternateBottomUp")
            SetLedStripArrangement(LedStripArrangementEnum::LeftRightAlternateBottomUp);
         else if (arrangement == "RightLeftAlternateTopDown")
            SetLedStripArrangement(LedStripArrangementEnum::RightLeftAlternateTopDown);
         else if (arrangement == "RightLeftAlternateBottomUp")
            SetLedStripArrangement(LedStripArrangementEnum::RightLeftAlternateBottomUp);
         else if (arrangement == "TopDownAlternateLeftRight")
            SetLedStripArrangement(LedStripArrangementEnum::TopDownAlternateLeftRight);
         else if (arrangement == "TopDownAlternateRightLeft")
            SetLedStripArrangement(LedStripArrangementEnum::TopDownAlternateRightLeft);
         else if (arrangement == "BottomUpAlternateLeftRight")
            SetLedStripArrangement(LedStripArrangementEnum::BottomUpAlternateLeftRight);
         else if (arrangement == "BottomUpAlternateRightLeft")
            SetLedStripArrangement(LedStripArrangementEnum::BottomUpAlternateRightLeft);
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