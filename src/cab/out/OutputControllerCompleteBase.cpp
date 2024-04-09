#include "OutputControllerCompleteBase.h"

namespace DOF
{

OutputControllerCompleteBase::OutputControllerCompleteBase() {}

OutputControllerCompleteBase::~OutputControllerCompleteBase() {}

void OutputControllerCompleteBase::Init(Cabinet* pCabinet) {}

void OutputControllerCompleteBase::Finish() {}

void OutputControllerCompleteBase::Update() {}

void OutputControllerCompleteBase::SetupOutputs()
{
  int numberOfOutputs = GetNumberOfConfiguredOutputs();
  /*if (Outputs == null)
  {

  }*/
}

}  // namespace DOF
