#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

namespace DOF
{

class Curve
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

public:
   const uint8_t* GetData() const { return m_data; }
   void SetData(const uint8_t* data);
   uint8_t MapValue(uint8_t curvePosition) const;
   uint8_t MapValue(int curvePosition) const;
   void SetCurve(CurveTypeEnum curveType);
   Curve();
   Curve(CurveTypeEnum curveType);

private:
   void BuildCurve(CurveTypeEnum curveType);
};

}