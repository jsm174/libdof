#pragma once

#include "../Output.h"

namespace DOF
{

class LedWizOutput : public Output
{
public:
   LedWizOutput();
   LedWizOutput(int ledWizOutputNumber);
   virtual ~LedWizOutput();

   int GetLedWizOutputNumber() const { return m_ledWizOutputNumber; }
   void SetLedWizOutputNumber(int value);

private:
   int m_ledWizOutputNumber;
};

}