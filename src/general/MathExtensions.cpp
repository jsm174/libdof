#include "MathExtensions.h"

namespace DOF
{

int MathExtensions::Limit(int value, int min, int max) { return std::max(min, std::min(max, value)); }

float MathExtensions::Limit(float value, float min, float max) { return std::max(min, std::min(max, value)); }

bool MathExtensions::IsBetween(int value, int min, int max) { return value >= min && value <= max; }

bool MathExtensions::IsIntegral(float value) { return value == std::floor(value); }

float MathExtensions::Floor(float value) { return std::floor(value); }

float MathExtensions::Ceiling(float value) { return std::ceil(value); }

int MathExtensions::RoundToInt(float value) { return static_cast<int>(std::round(value)); }

}