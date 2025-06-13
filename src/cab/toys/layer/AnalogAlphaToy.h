#pragma once

#include "../ToyBaseUpdatable.h"
#include "../IAnalogAlphaToy.h"
#include "../ISingleOutputToy.h"
#include "ILayerToy.h"
#include "../../../general/analog/AnalogAlpha.h"
#include "../../../general/Curve.h"
#include <vector>

namespace DOF
{

class IOutput;
class Cabinet;

class AnalogAlphaToy : public ToyBaseUpdatable, public IAnalogAlphaToy, public ISingleOutputToy
{
public:
   AnalogAlphaToy();
   virtual ~AnalogAlphaToy();

   virtual void Init(Cabinet* cabinet) override;
   virtual void Reset() override;
   virtual void Finish() override;
   virtual void UpdateToy() override;
   virtual int GetOutputCount() const override { return 1; }
   virtual void UpdateOutputs() override;
   virtual LayerDictionary<AnalogAlpha>& GetLayers() override { return m_layers; }
   virtual const LayerDictionary<AnalogAlpha>& GetLayers() const override { return m_layers; }
   void SetOutput(IOutput* output) { m_output = output; }
   IOutput* GetOutput() const { return m_output; }
   AnalogAlpha GetCurrentValue() { return AnalogAlpha(GetResultingValue(), 255); }

   virtual const std::string& GetOutputName() const override { return m_outputName; }
   virtual void SetOutputName(const std::string& name) override { m_outputName = name; }

   const std::string& GetFadingCurveName() const { return m_fadingCurveName; }
   void SetFadingCurveName(const std::string& name) { m_fadingCurveName = name; }

protected:
   int GetResultingValue();
   void InitFadingCurve(Cabinet* cabinet);

private:
   LayerDictionary<AnalogAlpha> m_layers;
   void InitOutputs(Cabinet* cabinet);

   IOutput* m_output;
   std::string m_outputName;
   std::string m_fadingCurveName;
   Curve* m_fadingCurve;
   Cabinet* m_cabinet;
};

}