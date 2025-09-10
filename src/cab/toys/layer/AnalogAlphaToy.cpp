#include "AnalogAlphaToy.h"
#include "../../Cabinet.h"
#include "../../out/IOutput.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include "../../CabinetOutputList.h"
#include "AlphaMappingTable.h"

#include <algorithm>

namespace DOF
{

AnalogAlphaToy::AnalogAlphaToy()
   : m_output(nullptr)
   , m_outputName("")
   , m_fadingCurveName("Linear")
   , m_fadingCurve(nullptr)
   , m_cabinet(nullptr)
{
}

AnalogAlphaToy::~AnalogAlphaToy()
{
   if (m_fadingCurve != nullptr)
   {
      delete m_fadingCurve;
      m_fadingCurve = nullptr;
   }
}

void AnalogAlphaToy::Init(Cabinet* cabinet)
{
   m_cabinet = cabinet;
   InitOutputs(cabinet);
   InitFadingCurve(cabinet);
}

void AnalogAlphaToy::InitOutputs(Cabinet* cabinet)
{
   if (cabinet->GetOutputs()->Contains(m_outputName))
      m_output = cabinet->GetOutputs()->GetByName(m_outputName);
   else
      m_output = nullptr;
}

void AnalogAlphaToy::Reset()
{
   m_layers.Clear();
   if (m_output != nullptr)
      m_output->SetOutput(0);
}

void AnalogAlphaToy::Finish()
{
   Log::Debug(StringExtensions::Build("Finishing AnalogAlphaToy: {0}", GetName()));
   Reset();
   if (m_fadingCurve != nullptr)
   {
      delete m_fadingCurve;
      m_fadingCurve = nullptr;
   }
   m_cabinet = nullptr;
}

void AnalogAlphaToy::UpdateToy() { UpdateOutputs(); }

void AnalogAlphaToy::UpdateOutputs()
{
   if (m_output != nullptr)
   {
      int resultingValue = GetResultingValue();
      Log::Debug(StringExtensions::Build("AnalogAlphaToy: {0}: {1}", m_output->GetName(), std::to_string(resultingValue)));

      if (m_fadingCurve != nullptr)
         m_output->SetOutput(m_fadingCurve->MapValue(resultingValue));
      else
         m_output->SetOutput(static_cast<uint8_t>(resultingValue));
   }
}

int AnalogAlphaToy::GetResultingValue()
{
   if (m_layers.GetLayerNumbers().size() > 0)
   {
      float value = 0;

      std::vector<int> layerNumbers = m_layers.GetLayerNumbers();
      for (int layerNumber : layerNumbers)
      {
         AnalogAlpha* layer = m_layers.GetLayer(layerNumber, 1);
         if (layer != nullptr)
         {
            const AnalogAlpha& layerValue = layer[0];
            int alpha = layerValue.GetAlpha();
            int val = layerValue.GetValue();
            if (alpha != 0)
            {
               float oldValue = AlphaMappingTable::AlphaMapping[255 - alpha][static_cast<int>(value)];
               float newValue = AlphaMappingTable::AlphaMapping[alpha][layerValue.GetValue()];
               value = oldValue + newValue;
            }
         }
      }

      return static_cast<int>(value);
   }
   else
   {
      return 0;
   }
}

void AnalogAlphaToy::InitFadingCurve(Cabinet* cabinet)
{
   if (m_fadingCurve != nullptr)
   {
      delete m_fadingCurve;
      m_fadingCurve = nullptr;
   }

   if (m_fadingCurveName == "Linear")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear);
   }
   else if (m_fadingCurveName == "Linear0To224")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear0To224);
   }
   else if (m_fadingCurveName == "Linear0To192")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear0To192);
   }
   else if (m_fadingCurveName == "Linear0To160")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear0To160);
   }
   else if (m_fadingCurveName == "Linear0To128")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear0To128);
   }
   else if (m_fadingCurveName == "Linear0To96")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear0To96);
   }
   else if (m_fadingCurveName == "Linear0To64")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear0To64);
   }
   else if (m_fadingCurveName == "Linear0To32")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear0To32);
   }
   else if (m_fadingCurveName == "Linear0To16")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear0To16);
   }
   else if (m_fadingCurveName == "InvertedLinear")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::InvertedLinear);
   }
   else if (m_fadingCurveName == "SwissLizardsLedCurve")
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::SwissLizardsLedCurve);
   }
   else
   {
      m_fadingCurve = new Curve(Curve::CurveTypeEnum::Linear);
   }
}

}