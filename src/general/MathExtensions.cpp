#include "MathExtensions.h"

namespace DOF
{

int MathExtensions::Limit(int value, int min, int max) { return std::max(min, std::min(max, value)); }

float MathExtensions::Limit(float value, float min, float max) { return std::max(min, std::min(max, value)); }

bool MathExtensions::IsBetween(int value, int min, int max) { return value >= min && value <= max; }

}