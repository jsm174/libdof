#include "Output.h"

namespace DOF
{

void Output::SetOutput(uint8_t value)
{
  uint8_t oldValue = GetOutput();
  IOutput::SetOutput(value);

  if (oldValue != value)
  {
    OnValueChanged();
  }
}

void Output::OnValueChanged()
{
  // if (ValueChanged != null)
  //{
  //   ValueChanged(this, new OutputEventArgs(this));
  // }
}

}  // namespace DOF
