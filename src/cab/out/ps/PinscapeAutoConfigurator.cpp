#include "PinscapeAutoConfigurator.h"

#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include "../../Cabinet.h"
#include "../../toys/ToyList.h"
#include "../../toys/lwequivalent/LedWizEquivalent.h"
#include "../OutputControllerList.h"
#include "Pinscape.h"
#include "PinscapeDevice.h"

namespace DOF
{

void PinscapeAutoConfigurator::AutoConfig(Cabinet* pCabinet)
{
   const int UnitBias = 50;

   std::vector<int> preconfigured;
   for (IOutputController* controller : *pCabinet->GetOutputControllers())
   {
      Pinscape* pinscape = dynamic_cast<Pinscape*>(controller);
      if (pinscape)
         preconfigured.push_back(pinscape->GetNumber());
   }

   std::vector<int> numbers;
   for (PinscapeDevice* device : Pinscape::AllDevices())
      numbers.push_back(device->GetUnitNo());

   for (int N : numbers)
   {
      if (std::find(preconfigured.begin(), preconfigured.end(), N) == preconfigured.end())
      {
         Pinscape* p = new Pinscape(N);
         if (!pCabinet->GetOutputControllers()->Contains(p->GetName()))
         {
            pCabinet->GetOutputControllers()->push_back(p);
            Log::Write(StringExtensions::Build("Detected and added Pinscape Controller Nr. {0} with name {1}", std::to_string(p->GetNumber()), p->GetName()));

            bool toyExists = false;
            int targetLedWizNumber = p->GetNumber() + UnitBias;

            for (IToy* toy : *pCabinet->GetToys())
            {
               LedWizEquivalent* lwe = dynamic_cast<LedWizEquivalent*>(toy);
               if (lwe && lwe->GetLedWizNumber() == targetLedWizNumber)
               {
                  toyExists = true;
                  break;
               }
            }

            if (!toyExists)
            {
               LedWizEquivalent* lwe = new LedWizEquivalent();
               lwe->SetLedWizNumber(p->GetNumber() + UnitBias);
               lwe->SetName(p->GetName() + " Equivalent");

               for (int i = 1; i <= p->GetNumberOfOutputs(); i++)
               {
                  LedWizEquivalentOutput* LWEO = new LedWizEquivalentOutput();

                  LWEO->SetOutputName(p->GetName() + "." + std::to_string(i));
                  LWEO->SetLedWizEquivalentOutputNumber(i);
                  lwe->GetOutputs().AddOutput(LWEO);
               }

               if (!pCabinet->GetToys()->Contains(lwe->GetName()))
               {
                  pCabinet->GetToys()->push_back(lwe);
                  Log::Write(StringExtensions::Build("Added LedwizEquivalent Nr. {0} with name {1} for Pinscape Controller Nr. {2}, {3} outputs", std::to_string(lwe->GetLedWizNumber()),
                     lwe->GetName(), std::to_string(p->GetNumber()), std::to_string(p->GetNumberOfOutputs())));
               }
            }
         }
      }
   }
}

}
