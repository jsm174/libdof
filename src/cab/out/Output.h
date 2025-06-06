#pragma once

#include "DOF/DOF.h"
#include "IOutput.h"

namespace DOF
{

class Output : public IOutput
{
public:
   Output() { }
   ~Output() { }

   void SetOutput(uint8_t value) override;

   void OnValueChanged();
};

} // namespace DOF
