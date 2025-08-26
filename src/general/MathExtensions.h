#pragma once

#include <algorithm>
#include <climits>
#include <cmath>

namespace DOF
{

class MathExtensions
{
public:
   static int Limit(int value, int min, int max);
   static float Limit(float value, float min, float max);
   static bool IsBetween(int value, int min, int max);
   static bool IsIntegral(float value);
   static float Floor(float value);
   static float Ceiling(float value);
   static int RoundToInt(float value);
};

}