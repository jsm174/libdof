#pragma once

#include <cstdint>

namespace DOF
{

struct PixelData
{
   uint8_t red;
   uint8_t green;
   uint8_t blue;
   uint8_t alpha;

   PixelData()
      : red(0)
      , green(0)
      , blue(0)
      , alpha(0)
   {
   }
   PixelData(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
      : red(r)
      , green(g)
      , blue(b)
      , alpha(a)
   {
   }

   bool operator==(const PixelData& other) const { return red == other.red && green == other.green && blue == other.blue && alpha == other.alpha; }

   bool operator!=(const PixelData& other) const { return !(*this == other); }
};

}