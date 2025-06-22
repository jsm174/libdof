#include "DMXOutput.h"
#include "../../../general/StringExtensions.h"
#include "../../../general/MathExtensions.h"
#include <stdexcept>

namespace DOF
{

DMXOutput::DMXOutput()
   : m_dmxChannel(0)
{
}

DMXOutput::~DMXOutput() { }

void DMXOutput::SetDmxChannel(int value)
{
   if (m_dmxChannel != value)
   {
      if (!MathExtensions::IsBetween(value, 1, 512))
      {
         throw std::runtime_error(StringExtensions::Build("Dmx channels numbers must be in the range of 1-512. The supplied number {0} is out of range.", std::to_string(value)));
      }

      if (GetName().empty() || GetName() == StringExtensions::Build("DmxChannel {0:000}", std::to_string(m_dmxChannel)))
      {
         SetName(StringExtensions::Build("DmxChannel {0:00}", std::to_string(value)));
      }

      m_dmxChannel = value;
   }
}

}