#include "PinscapeAutoConfigurator.h"

#include "../../../Log.h"
#include "../../Cabinet.h"
#include "../OutputControllerList.h"
#include "Pinscape.h"
#include "PinscapeDevice.h"

namespace DOF
{

void PinscapeAutoConfigurator::AutoConfig(Cabinet* pCabinet)
{
  int UnitBias = 50;

  for (PinscapeDevice* pDevice : Pinscape::GetAllDevices())
  {
    int unitNo = pDevice->GetUnitNo();
    if (unitNo)
    {
      Pinscape* pPinscape = new Pinscape(unitNo);

      if (!pCabinet->GetOutputControllers()->Contains(pPinscape->GetName()))
      {
        pCabinet->GetOutputControllers()->push_back(pPinscape);

        Log::Write("Detected and added Pinscape Controller Nr. %d with name %s", pPinscape->GetNumber(),
                   pPinscape->GetName().c_str());

        /*if (!Cabinet.Toys.Any(T = > T is LedWizEquivalent && ((LedWizEquivalent)T).LedWizNumber == p.Number +
        UnitBias))
        {
          LedWizEquivalent LWE = new LedWizEquivalent();
          LWE.LedWizNumber = p.Number + UnitBias;
          LWE.Name = "{0} Equivalent".Build(p.Name);

          for (int i = 1; i <= p.NumberOfOutputs; i++)
          {
            LedWizEquivalentOutput LWEO = new LedWizEquivalentOutput(){OutputName = "{0}\\{0}.{1:00}".Build(p.Name, i),
                                                                       LedWizEquivalentOutputNumber = i};
            LWE.Outputs.Add(LWEO);
          }

          if (!Cabinet.Toys.Contains(LWE.Name))
          {
            Cabinet.Toys.Add(LWE);
            Log.Write("Added LedwizEquivalent Nr. {0} with name {1} for Pinscape Controller Nr. {2}".Build(
                          LWE.LedWizNumber, LWE.Name, p.Number) +
                      ", {0}".Build(p.NumberOfOutputs));
          }
        }*/
      }
    }
  }
}

void PinscapeAutoConfigurator::Test(Pinscape* pPinscape)
{
  int noOutputs = pPinscape->GetNumberOfConfiguredOutputs();

  if (noOutputs > 0)
  {
    uint8_t outputs[noOutputs];

    auto start = std::chrono::steady_clock::now();
    int direction = 1;
    int value = 0;
    int stepSize = 5;

    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(10))
    {
      for (auto& output : outputs)
      {
        output = static_cast<uint8_t>(value);
      }

      pPinscape->UpdateOutputs(outputs);

      value += stepSize * direction;
      if (value >= 255 || value <= 0)
      {
        direction *= -1;
        value = std::max(0, std::min(value, 255));
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    pPinscape->Finish();
  }
}

}  // namespace DOF
