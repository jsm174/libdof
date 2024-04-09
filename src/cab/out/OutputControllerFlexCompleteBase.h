#pragma once

#include "DOF/DOF.h"
#include "OutputControllerCompleteBase.h"

namespace DOF
{

class OutputControllerFlexCompleteBase : public OutputControllerCompleteBase
{
 public:
  OutputControllerFlexCompleteBase();
  ~OutputControllerFlexCompleteBase();

  virtual int GetNumberOfOutputs() { return m_numberOfOutputs; }
  virtual void SetNumberOfOutputs(int numberOfOutputs);

  int GetNumberOfConfiguredOutputs() override;

 private:
  int m_numberOfOutputs;
};

}  // namespace DOF
