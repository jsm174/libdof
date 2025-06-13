#pragma once

#include "../ToyBaseUpdatable.h"
#include "../IRGBAToy.h"
#include "ILayerToy.h"
#include "../../../general/color/RGBAColor.h"
#include <vector>

namespace DOF
{

class IOutput;

class RGBAToy : public ToyBaseUpdatable, public ILayerToy<RGBAColor>
{
public:
   RGBAToy();
   virtual ~RGBAToy();

   virtual void Init(Cabinet* cabinet) override;
   virtual void Reset() override;
   virtual void Finish() override;
   virtual void UpdateToy() override;
   virtual int GetOutputCount() const { return 3; }
   virtual void UpdateOutputs() override;
   virtual LayerDictionary<RGBAColor>& GetLayers() override { return m_layers; }
   virtual const LayerDictionary<RGBAColor>& GetLayers() const override { return m_layers; }
   void SetRedOutput(IOutput* output) { m_redOutput = output; }
   void SetGreenOutput(IOutput* output) { m_greenOutput = output; }
   void SetBlueOutput(IOutput* output) { m_blueOutput = output; }

   IOutput* GetRedOutput() const { return m_redOutput; }
   IOutput* GetGreenOutput() const { return m_greenOutput; }
   IOutput* GetBlueOutput() const { return m_blueOutput; }

   RGBAColor GetCurrentColor() const { return m_currentColor; }

protected:
   void MergeLayers();

private:
   LayerDictionary<RGBAColor> m_layers;
   RGBAColor m_currentColor;

   IOutput* m_redOutput;
   IOutput* m_greenOutput;
   IOutput* m_blueOutput;
};

}