#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include "generic/IXmlSerializable.h"

namespace DOF
{

class Curve : public IXmlSerializable
{
public:
   enum class CurveTypeEnum
   {
      Linear,
      Linear0To224,
      Linear0To192,
      Linear0To160,
      Linear0To128,
      Linear0To96,
      Linear0To64,
      Linear0To32,
      Linear0To16,
      InvertedLinear,
      SwissLizardsLedCurve
   };

private:
   uint8_t m_data[256];
   std::string m_name;

public:
   const uint8_t* GetData() const { return m_data; }
   void SetData(const uint8_t* data);
   uint8_t MapValue(uint8_t curvePosition) const;
   uint8_t MapValue(int curvePosition) const;
   void SetCurve(CurveTypeEnum curveType);
   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name) { m_name = name; }
   Curve();
   Curve(CurveTypeEnum curveType);

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "Curve"; }

private:
   void BuildCurve(CurveTypeEnum curveType);
   CurveTypeEnum* GetCurveTypeEnum() const;
   static std::string CurveTypeEnumToString(CurveTypeEnum curveType);
   static CurveTypeEnum StringToCurveTypeEnum(const std::string& str);
};

}