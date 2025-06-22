#include "IOutput.h"

namespace DOF
{

const std::string OutputEventArgs::s_emptyString;

uint8_t OutputEventArgs::GetValue() const { return m_output ? m_output->GetOutput() : 0; }

const std::string& OutputEventArgs::GetName() const { return m_output ? m_output->GetName() : s_emptyString; }

IOutput::IOutput()
   : m_value(0)
   , m_number(0)
{
}

IOutput::~IOutput() { }

void IOutput::OnValueChanged() { ValueChanged.Invoke(this, OutputEventArgs(this)); }

}
