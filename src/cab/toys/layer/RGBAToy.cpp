#include "RGBAToy.h"
#include "../../Cabinet.h"
#include "../../out/IOutput.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"

#include <algorithm>

namespace DOF
{

RGBAToy::RGBAToy()
   : m_redOutput(nullptr)
   , m_greenOutput(nullptr)
   , m_blueOutput(nullptr)
{
}

RGBAToy::~RGBAToy() { }

void RGBAToy::Init(Cabinet* cabinet)
{
   if (cabinet == nullptr)
   {
      Log::Warning("RGBAToy::Init - Cabinet is null");
      return;
   }

   Log::Write(StringExtensions::Build("Initializing RGBAToy: {0}", GetName()));

   Reset();
}

void RGBAToy::Reset()
{

   m_currentColor = RGBAColor(0, 0, 0, 255);
   UpdateOutputs();
}

void RGBAToy::Finish()
{
   Log::Write(StringExtensions::Build("Finishing RGBAToy: {0}", GetName()));
   Reset();
}

void RGBAToy::UpdateToy()
{
   MergeLayers();

   UpdateOutputs();
}

void RGBAToy::UpdateOutputs()
{
   if (m_redOutput != nullptr)
      m_redOutput->SetOutput(static_cast<uint8_t>(m_currentColor.GetRed()));

   if (m_greenOutput != nullptr)
      m_greenOutput->SetOutput(static_cast<uint8_t>(m_currentColor.GetGreen()));

   if (m_blueOutput != nullptr)
      m_blueOutput->SetOutput(static_cast<uint8_t>(m_currentColor.GetBlue()));
}

void RGBAToy::MergeLayers()
{
   m_currentColor = RGBAColor(0, 0, 0, 255);

   std::vector<int> layerNumbers = m_layers.GetLayerNumbers();
   std::sort(layerNumbers.begin(), layerNumbers.end());

   for (int layerNumber : layerNumbers)
   {
      RGBAColor* layer = m_layers.GetLayer(layerNumber, 1);
      if (layer != nullptr)
      {
         const RGBAColor& layerColor = layer[0];

         if (layerColor.GetAlpha() > 0)
         {
            float alpha = layerColor.GetAlpha() / 255.0f;
            float invAlpha = 1.0f - alpha;

            int red = static_cast<int>(m_currentColor.GetRed() * invAlpha + layerColor.GetRed() * alpha);
            int green = static_cast<int>(m_currentColor.GetGreen() * invAlpha + layerColor.GetGreen() * alpha);
            int blue = static_cast<int>(m_currentColor.GetBlue() * invAlpha + layerColor.GetBlue() * alpha);

            m_currentColor.SetRGBA(red, green, blue, 255);
         }
      }
   }
}

}