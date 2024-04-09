#pragma once

#include "DOF/DOF.h"

namespace DOF
{

class IOutput
{
 public:
  IOutput() {}
  ~IOutput() {}

  virtual uint8_t GetOutput() { return m_value; }
  virtual void SetOutput(uint8_t value) { m_value = value; }
  int GetNumber() const { return m_number; }
  void SetNumber(int number) { m_number = number; }

 private:
  uint8_t m_value;
  int m_number;
  // event Output.ValueChangedEventHandler ValueChanged;
};

}  // namespace DOF
