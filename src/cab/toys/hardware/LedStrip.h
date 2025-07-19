#pragma once

#include "../ToyBaseUpdatable.h"
#include "../IMatrixToy.h"
#include "../layer/RGBOrderEnum.h"
#include "../layer/StripArrangementEnum.h"
#include "../layer/MatrixDictionaryBase.h"
#include "../../../general/color/RGBAColor.h"
#include "../../../general/Curve.h"
#include <tinyxml2/tinyxml2.h>
#include <vector>
#include <string>
#include <memory>

namespace DOF
{

class IOutput;
class ISupportsSetValues;
class Cabinet;

class LedStrip : public ToyBaseUpdatable, public IMatrixToy<RGBAColor>
{
public:
   LedStrip();
   virtual ~LedStrip();

   virtual int GetWidth() const override { return m_width; }
   void SetWidth(int value);
   virtual int GetHeight() const override { return m_height; }
   void SetHeight(int value);
   int GetNumberOfLeds() const { return m_width * m_height; }
   int GetNumberOfOutputs() const { return GetNumberOfLeds() * 3; }
   LedStripArrangementEnum GetLedStripArrangement() const { return m_ledStripArrangement; }
   void SetLedStripArrangement(LedStripArrangementEnum value) { m_ledStripArrangement = value; }
   RGBOrderEnum GetColorOrder() const { return m_colorOrder; }
   void SetColorOrder(RGBOrderEnum value) { m_colorOrder = value; }
   int GetFirstLedNumber() const { return m_firstLedNumber; }
   void SetFirstLedNumber(int value);
   const std::string& GetFadingCurveName() const { return m_fadingCurveName; }
   void SetFadingCurveName(const std::string& name);
   float GetBrightnessGammaCorrection() const { return m_brightnessGammaCorrection; }
   void SetBrightnessGammaCorrection(float value);
   int GetBrightness() const { return static_cast<int>((m_brightness * 100.0f) + 0.5f); }
   void SetBrightness(int value);
   const std::string& GetOutputControllerName() const { return m_outputControllerName; }
   void SetOutputControllerName(const std::string& name) { m_outputControllerName = name; }

   MatrixDictionaryBase<RGBAColor>& GetLayers() { return m_layers; }
   const MatrixDictionaryBase<RGBAColor>& GetLayers() const { return m_layers; }

   virtual void Init(Cabinet* cabinet) override;
   virtual void Reset() override;
   virtual void Finish() override;
   virtual void UpdateToy() override;

   virtual RGBAColor* GetLayer(int layerNr) override;
   virtual RGBAColor GetElement(int layerNr, int x, int y) override;
   virtual void SetElement(int layerNr, int x, int y, const RGBAColor& value) override;

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const;
   virtual bool FromXml(const tinyxml2::XMLElement* element);
   virtual std::string GetXmlElementName() const { return "LedStrip"; }

protected:
   virtual void UpdateOutputs() override;

private:
   int m_width;
   int m_height;
   LedStripArrangementEnum m_ledStripArrangement;
   RGBOrderEnum m_colorOrder;
   int m_firstLedNumber;
   std::string m_fadingCurveName;
   Curve* m_fadingCurve;
   float m_brightnessGammaCorrection;
   float m_brightness;
   std::string m_outputControllerName;
   ISupportsSetValues* m_outputController;

   MatrixDictionaryBase<RGBAColor> m_layers;
   std::vector<std::vector<int>> m_outputMappingTable;
   std::vector<uint8_t> m_outputData;
   Cabinet* m_cabinet;

   void InitFadingCurve(Cabinet* cabinet);
   void BuildMappingTables();
   Curve GetFadingTableFromPercent(int outputPercent) const;
   void SetOutputData();
   int CalculateLedNumber(int x, int y) const;
   void ApplyColorOrder(uint8_t& r, uint8_t& g, uint8_t& b) const;
   uint8_t ApplyFadingCurve(uint8_t value) const;
};

}