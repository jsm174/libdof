#pragma once

#include "DOF/DOF.h"
#include "IOutputController.h"
#include "OutputList.h"
#include "IOutput.h"

namespace DOF
{

class OutputControllerBase : public virtual IOutputController
{
public:
   OutputControllerBase();
   virtual ~OutputControllerBase();

   virtual OutputList* GetOutputs() override { return m_outputs; }
   virtual const OutputList* GetOutputs() const override { return m_outputs; }
   virtual void SetOutputs(OutputList* outputs);
   virtual void Init(Cabinet* cabinet) override = 0;
   virtual void Finish() override = 0;
   virtual void Update() override = 0;

protected:
   virtual void OnOutputValueChanged(IOutput* output) = 0;

private:
   OutputList* m_outputs;

   void OnOutputsValueChanged(const OutputEventArgs& args);
};

}
