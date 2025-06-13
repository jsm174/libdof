#include "IOutput.h"

namespace DOF
{

IOutput::IOutput()
   : m_value(0)
   , m_number(0)
{
}

IOutput::~IOutput() { }

void IOutput::OnValueChanged() { ValueChanged.Invoke(this, OutputEventArgs(this)); }

}
