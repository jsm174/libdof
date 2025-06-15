#pragma once

#include <algorithm>

namespace DOF
{

class AnalogAlpha
{
private:
   int m_value;
   int m_alpha;

public:
   int GetValue() const { return m_value; }
   void SetValue(int value) { m_value = std::clamp(value, 0, 255); }
   int GetAlpha() const { return m_alpha; }
   void SetAlpha(int value) { m_alpha = std::clamp(value, 0, 255); }
   bool IsZero() const { return m_value == 0; }
   bool operator==(const AnalogAlpha& other) const;
   bool operator!=(const AnalogAlpha& other) const { return !(*this == other); }
   AnalogAlpha();
   AnalogAlpha(int value, int alpha = 255);
};

}