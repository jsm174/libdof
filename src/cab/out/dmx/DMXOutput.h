#pragma once

#include "../Output.h"
#include "../IOutput.h"

namespace DOF
{

class DMXOutput : public Output
{
public:
   DMXOutput();
   virtual ~DMXOutput();

   int GetDmxChannel() const { return m_dmxChannel; }
   void SetDmxChannel(int value);

private:
   int m_dmxChannel;
};

}