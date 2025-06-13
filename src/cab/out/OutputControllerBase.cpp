#include "OutputControllerBase.h"

namespace DOF
{

OutputControllerBase::OutputControllerBase()
   : m_outputs(new OutputList())
{
   m_outputs->OutputValueChanged += [this](void* sender, const OutputEventArgs& args) { OnOutputsValueChanged(args); };
}

OutputControllerBase::~OutputControllerBase() { delete m_outputs; }

void OutputControllerBase::SetOutputs(OutputList* outputs)
{
   if (m_outputs != outputs)
   {
      delete m_outputs;
      m_outputs = outputs;

      if (m_outputs)
      {
         m_outputs->OutputValueChanged += [this](void* sender, const OutputEventArgs& args) { OnOutputsValueChanged(args); };
      }
   }
}

void OutputControllerBase::OnOutputsValueChanged(const OutputEventArgs& args) { OnOutputValueChanged(args.GetOutput()); }

}
