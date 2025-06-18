#include "Curve.h"
#include "MathExtensions.h"
#include <algorithm>
#include <cstring>

namespace DOF
{

void Curve::SetData(const uint8_t* data)
{
   if (data != nullptr)
   {
      std::memcpy(m_data, data, 256);
   }
}

uint8_t Curve::MapValue(uint8_t curvePosition) const { return m_data[curvePosition]; }

uint8_t Curve::MapValue(int curvePosition) const
{
   if (curvePosition < 0 || curvePosition > 255)
   {
      throw std::out_of_range("Position " + std::to_string(curvePosition) + " does not exist for the curve.");
   }
   return m_data[curvePosition];
}

void Curve::SetCurve(CurveTypeEnum curveType) { BuildCurve(curveType); }

Curve::Curve() { BuildCurve(CurveTypeEnum::Linear); }

Curve::Curve(CurveTypeEnum curveType) { BuildCurve(curveType); }

void Curve::BuildCurve(CurveTypeEnum curveType)
{
   switch (curveType)
   {
   case CurveTypeEnum::SwissLizardsLedCurve:
   {
      uint8_t swissLizardCurve[256] = { 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17,
         18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 27, 28, 29, 30, 30, 31, 32, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 48, 49, 50, 51,
         52, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 88, 89, 90, 91, 92, 93, 94, 95, 96,
         97, 98, 99, 100, 102, 103, 104, 105, 106, 107, 108, 109, 110, 112, 113, 114, 115, 116, 117, 119, 120, 121, 122, 123, 124, 126, 127, 128, 129, 130, 132, 133, 134, 135, 136, 138, 139,
         140, 141, 142, 144, 145, 146, 147, 149, 150, 151, 152, 154, 155, 156, 158, 159, 160, 161, 162, 164, 165, 167, 168, 169, 171, 172, 173, 174, 176, 177, 178, 180, 181, 182, 184, 185,
         187, 188, 189, 191, 192, 193, 195, 196, 197, 199, 200, 202, 203, 204, 206, 207, 208, 210, 211, 213, 214, 216, 217, 218, 220, 221, 223, 224, 226, 227, 228, 230, 231, 233, 234, 236,
         237, 239, 240, 242, 243, 245, 246, 248, 249, 251, 252, 254, 255 };
      std::memcpy(m_data, swissLizardCurve, 256);
      break;
   }
   case CurveTypeEnum::InvertedLinear:
      for (int i = 0; i <= 255; i++)
      {
         m_data[i] = static_cast<uint8_t>(255 - i);
      }
      break;
   case CurveTypeEnum::Linear0To224:
      for (int i = 0; i <= 255; i++)
      {
         m_data[i] = static_cast<uint8_t>(MathExtensions::Limit(static_cast<int>(224.0 / 255.0 * i), 0, 255));
      }
      break;
   case CurveTypeEnum::Linear0To192:
      for (int i = 0; i <= 255; i++)
      {
         m_data[i] = static_cast<uint8_t>(MathExtensions::Limit(static_cast<int>(192.0 / 255.0 * i), 0, 255));
      }
      break;
   case CurveTypeEnum::Linear0To160:
      for (int i = 0; i <= 255; i++)
      {
         m_data[i] = static_cast<uint8_t>(MathExtensions::Limit(static_cast<int>(160.0 / 255.0 * i), 0, 255));
      }
      break;
   case CurveTypeEnum::Linear0To128:
      for (int i = 0; i <= 255; i++)
      {
         m_data[i] = static_cast<uint8_t>(MathExtensions::Limit(static_cast<int>(128.0 / 255.0 * i), 0, 255));
      }
      break;
   case CurveTypeEnum::Linear0To96:
      for (int i = 0; i <= 255; i++)
      {
         m_data[i] = static_cast<uint8_t>(MathExtensions::Limit(static_cast<int>(96.0 / 255.0 * i), 0, 255));
      }
      break;
   case CurveTypeEnum::Linear0To64:
      for (int i = 0; i <= 255; i++)
      {
         m_data[i] = static_cast<uint8_t>(MathExtensions::Limit(static_cast<int>(64.0 / 255.0 * i), 0, 255));
      }
      break;
   case CurveTypeEnum::Linear0To32:
      for (int i = 0; i <= 255; i++)
      {
         m_data[i] = static_cast<uint8_t>(MathExtensions::Limit(static_cast<int>(32.0 / 255.0 * i), 0, 255));
      }
      break;
   case CurveTypeEnum::Linear0To16:
      for (int i = 0; i <= 255; i++)
      {
         m_data[i] = static_cast<uint8_t>(MathExtensions::Limit(static_cast<int>(16.0 / 255.0 * i), 0, 255));
      }
      break;
   case CurveTypeEnum::Linear:
   default:
      for (int i = 0; i <= 255; i++)
      {
         m_data[i] = static_cast<uint8_t>(i);
      }
      break;
   }
}

}