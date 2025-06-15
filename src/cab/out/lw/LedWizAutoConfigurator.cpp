#include "LedWizAutoConfigurator.h"

#include "../../../Log.h"
#include "../../Cabinet.h"
#include "../../toys/ToyList.h"
#include "../../toys/lwequivalent/LedWizEquivalent.h"
#include "../OutputControllerList.h"
#include "LedWiz.h"
#include "../../../general/StringExtensions.h"

namespace DOF
{

LedWizAutoConfigurator::LedWizAutoConfigurator() { }

LedWizAutoConfigurator::~LedWizAutoConfigurator() { }

void LedWizAutoConfigurator::AutoConfig(Cabinet* pCabinet)
{
   std::vector<int> preconfigured;
   for (IOutputController* controller : *pCabinet->GetOutputControllers())
   {
      LedWiz* ledwiz = dynamic_cast<LedWiz*>(controller);
      if (ledwiz)
         preconfigured.push_back(ledwiz->GetNumber());
   }

   std::vector<int> numbers = LedWiz::GetLedwizNumbers();
   for (int n : numbers)
   {
      if (std::find(preconfigured.begin(), preconfigured.end(), n) == preconfigured.end())
      {
         LedWiz* lw = new LedWiz(n);
         if (!pCabinet->GetOutputControllers()->Contains(lw->GetName()))
         {
            pCabinet->GetOutputControllers()->push_back(lw);
            Log::Write(StringExtensions::Build("Detected and added LedWiz Nr. {0} with name {1}", std::to_string(lw->GetNumber()), lw->GetName()));

            bool toyExists = false;
            for (IToy* toy : *pCabinet->GetToys())
            {
               LedWizEquivalent* lwe = dynamic_cast<LedWizEquivalent*>(toy);
               if (lwe && lwe->GetLedWizNumber() == lw->GetNumber())
               {
                  toyExists = true;
                  break;
               }
            }

            if (!toyExists)
            {
               LedWizEquivalent* lwe = new LedWizEquivalent();
               lwe->SetLedWizNumber(lw->GetNumber());
               lwe->SetName(lw->GetName() + " Equivalent");

               for (int i = 1; i <= 32; i++)
               {
                  LedWizEquivalentOutput* lweo = new LedWizEquivalentOutput();
                  lweo->SetOutputName(lw->GetName() + "." + std::to_string(i));
                  lweo->SetLedWizEquivalentOutputNumber(i);
                  lwe->GetOutputs().AddOutput(lweo);
               }

               if (!pCabinet->GetToys()->Contains(lwe->GetName()))
               {
                  pCabinet->GetToys()->push_back(lwe);
                  Log::Write(StringExtensions::Build(
                     "Added LedwizEquivalent Nr. {0} with name {1} for Ledwiz Nr. {2}", std::to_string(lwe->GetLedWizNumber()), lwe->GetName(), std::to_string(lw->GetNumber())));
               }
            }
         }
      }
   }
}

}