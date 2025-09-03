#include "PacDriveAutoConfigurator.h"
#include "PacDrive.h"
#include "PacDriveSingleton.h"
#include "../../Cabinet.h"
#include "../OutputControllerList.h"
#include "../../toys/ToyList.h"
#include "../../toys/lwequivalent/LedWizEquivalent.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include <algorithm>

namespace DOF
{

void PacDriveAutoConfigurator::AutoConfig(Cabinet* cabinet)
{
   if (!cabinet)
      return;

   Log::Write("PacDrive auto-configuration starting");

   int index = PacDriveSingleton::GetInstance().PacDriveGetIndex();

   if (index >= 0)
   {
      bool found = false;
      for (IOutputController* oc : *cabinet->GetOutputControllers())
      {
         PacDrive* pacDrive = dynamic_cast<PacDrive*>(oc);
         if (pacDrive)
         {
            found = true;
            break;
         }
      }

      if (!found)
      {
         PacDrive* pacDrive = new PacDrive();

         if (!cabinet->GetOutputControllers()->Contains(pacDrive->GetName()))
         {
            cabinet->GetOutputControllers()->push_back(pacDrive);

            Log::Write("Detected and added PacDrive");

            bool toyFound = false;
            for (IToy* toy : *cabinet->GetToys())
            {
               LedWizEquivalent* lwe = dynamic_cast<LedWizEquivalent*>(toy);
               if (lwe && lwe->GetLedWizNumber() == 19)
               {
                  toyFound = true;
                  break;
               }
            }

            if (!toyFound)
            {
               LedWizEquivalent* lwe = new LedWizEquivalent();
               lwe->SetLedWizNumber(19);
               lwe->SetName(StringExtensions::Build("{0} Equivalent", pacDrive->GetName()));

               for (int i = 1; i <= 16; i++)
               {
                  LedWizEquivalentOutput* lweo = new LedWizEquivalentOutput();
                  lweo->SetOutputName(StringExtensions::Build("{0}.{1:00}", pacDrive->GetName(), std::to_string(i)));
                  lweo->SetLedWizEquivalentOutputNumber(i);
                  lwe->GetOutputs().AddOutput(lweo);
               }

               if (!cabinet->GetToys()->Contains(lwe->GetName()))
               {
                  cabinet->GetToys()->AddToy(lwe);
                  Log::Write(StringExtensions::Build("Added LedwizEquivalent Nr. {0} with name {1} for PacDrive", std::to_string(lwe->GetLedWizNumber()), lwe->GetName()));
               }
            }
         }
      }
   }
}

}