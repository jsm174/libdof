#include "OutputControllerFlexCompleteBase.h"

namespace DOF
{

OutputControllerFlexCompleteBase::OutputControllerFlexCompleteBase() { }

OutputControllerFlexCompleteBase::~OutputControllerFlexCompleteBase() { }

void OutputControllerFlexCompleteBase::SetNumberOfOutputs(int numberOfOutputs)
{
   m_numberOfOutputs = numberOfOutputs;
   OutputControllerCompleteBase::SetupOutputs();
}

int OutputControllerFlexCompleteBase::GetNumberOfConfiguredOutputs() { return m_numberOfOutputs; }

}
