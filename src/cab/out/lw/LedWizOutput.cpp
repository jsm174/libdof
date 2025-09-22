#include "LedWizOutput.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"

namespace DOF
{

LedWizOutput::LedWizOutput()
   : m_ledWizOutputNumber(1)
{
}

LedWizOutput::LedWizOutput(int ledWizOutputNumber)
   : LedWizOutput()
{
   SetLedWizOutputNumber(ledWizOutputNumber);
   SetNumber(ledWizOutputNumber);
   SetName(StringExtensions::Build("LedWizOutput {0:00}", std::to_string(ledWizOutputNumber)));
   SetOutput(0);
}

LedWizOutput::~LedWizOutput() { }

void LedWizOutput::SetLedWizOutputNumber(int value)
{
   if (value < 1 || value > 32)
   {
      Log::Exception(StringExtensions::Build("LedWiz output numbers must be in the range of 1-32. The supplied number {0} is out of range.", std::to_string(value)));
      return;
   }

   if (m_ledWizOutputNumber != value)
   {
      if (GetName().empty() || GetName() == StringExtensions::Build("LedWizOutput {0:00}", std::to_string(m_ledWizOutputNumber)))
      {
         SetName(StringExtensions::Build("LedWizOutput {0:00}", std::to_string(value)));
      }
      m_ledWizOutputNumber = value;
   }
}

}