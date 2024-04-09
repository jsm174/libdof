#pragma once

#include "DOF/DOF.h"
#include "IOutputController.h"

namespace DOF
{

class OutputControllerCompleteBase : public IOutputController
{
 public:
  OutputControllerCompleteBase();
  ~OutputControllerCompleteBase();

  virtual void Init(Cabinet* pCabinet) override;
  virtual void Finish() override;
  void Update() override;

 protected:
  void SetupOutputs();

  virtual int GetNumberOfConfiguredOutputs() = 0;
};

}  // namespace DOF
