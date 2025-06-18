#pragma once

#include <algorithm>
#include <climits>

namespace DOF
{

class MathExtensions
{
public:
   static int Limit(int value, int min, int max);
   static float Limit(float value, float min, float max);
   static bool IsBetween(int value, int min, int max);
};

}