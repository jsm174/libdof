#include "PixelData.h"
#include "../color/RGBAColor.h"

namespace DOF
{

RGBAColor PixelData::GetRGBAColor() const { return RGBAColor(red, green, blue, alpha); }

}