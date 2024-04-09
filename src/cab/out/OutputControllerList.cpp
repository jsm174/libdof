#include "OutputControllerList.h"

#include "../../Logger.h"

namespace DOF
{
void OutputControllerList::WriteXml() { Log("Not implemented"); }

void OutputControllerList::ReadXml() { Log("Not implemented"); }

void OutputControllerList::Init(Cabinet* pCabinet)
{
  Log("Initializing output controllers");

  for (IOutputController* pController : *this) pController->Init(pCabinet);

  Log("Output controllers initialized");
}

void OutputControllerList::Finish()
{
  Log("Finishing output controllers");

  for (IOutputController* pController : *this) pController->Finish();

  Log("Output controllers finished");
}

void OutputControllerList::Update()
{
  for (IOutputController* pController : *this) pController->Update();
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

}  // namespace DOF
