#include "Output.h"

namespace DOF
{

void Output::SetOutput(uint8_t value)
{
   uint8_t oldValue = GetOutput();
   IOutput::SetOutput(value);

   if (oldValue != value)
      OnValueChanged();
}

}
