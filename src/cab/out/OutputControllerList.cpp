#include "OutputControllerList.h"

#include "../../Log.h"

namespace DOF
{
void OutputControllerList::WriteXml() { Log::Write("Not implemented"); }

void OutputControllerList::ReadXml() { Log::Write("Not implemented"); }

void OutputControllerList::Init(Cabinet* pCabinet)
{
   Log::Write("Initializing output controllers");

   for (IOutputController* pController : *this)
      pController->Init(pCabinet);

   Log::Write("Output controllers initialized");
}

void OutputControllerList::Finish()
{
   Log::Write("Finishing output controllers");

   for (IOutputController* pController : *this)
      pController->Finish();

   Log::Write("Output controllers finished");
}

void OutputControllerList::Update()
{
   for (IOutputController* pController : *this)
      pController->Update();
}

bool OutputControllerList::Contains(const std::string& name) const
{
   for (IOutputController* pController : *this)
   {
      if (pController->GetName() == name)
      {
         return true;
      }
   }
   return false;
}

} // namespace DOF
