#pragma once

namespace DOF
{

class OutputControllerCompleteBase
{
 public:
  OutputControllerCompleteBase();
  ~OutputControllerCompleteBase();

  virtual void Init(/*Cabinet*/);
  virtual void Finish();

 protected:
  void SetupOutputs();

  virtual int GetNumberOfConfiguredOutputs() = 0;
};

}  // namespace DOF
