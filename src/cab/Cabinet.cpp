#include "Cabinet.h"

#include "../Log.h"
#include "CabinetOutputList.h"
#include "out/IAutoConfigOutputController.h"
#include "out/OutputControllerList.h"
#include "toys/ToyList.h"

#ifdef __HIDAPI__
#include <hidapi.h>

#include "out/ps/Pinscape.h"
#include "out/ps/PinscapeAutoConfigurator.h"
#endif

namespace DOF
{

Cabinet::Cabinet()
{
  m_autoConfigEnabled = true;
  m_pOutputControllers = new OutputControllerList();
  m_pOutputs = new CabinetOutputList(this);
  m_pToys = new ToyList();
  // m_pColors = new ColorList();

#ifdef __HIDAPI__
  hid_init();

  Pinscape::Initialize();
#endif
}

Cabinet::~Cabinet()
{
#ifdef __HIDAPI__
  hid_exit();
#endif
}

void Cabinet::AutoConfig()
{
  Log::Write("Cabinet auto configuration started");

#ifdef __HIDAPI__
  IAutoConfigOutputController* items[] = {new PinscapeAutoConfigurator()};

  for (auto& item : items)
  {
    item->AutoConfig(this);
  }
#endif

  Log::Write("Cabinet auto configuration finished");
}

std::string Cabinet::GetConfigXml()
{
  Log::Write("Not implemented");

  return "";
}

void Cabinet::SaveConfigXml(const std::string& szFileName) { Log::Write("Not implemented"); }

Cabinet* Cabinet::GetCabinetFromConfigXmlFile(const std::string& szFileName)
{
  Log::Write("Not implemented");

  return nullptr;
}

bool Cabinet::TestCabinetConfigXmlFile(const std::string& szFileName)
{
  Log::Write("Not implemented");

  return false;
}

void Cabinet::Init(ICabinetOwner* pCabinetOwner)
{
  Log::Write("Initializing cabinet");
  m_pOwner = pCabinetOwner;
  m_pOutputControllers->Init(this);
  m_pToys->Init(this);

  Log::Write("Cabinet initialized");
}

void Cabinet::Update()
{
  m_pToys->UpdateOutputs();
  m_pOutputControllers->Update();
}

void Cabinet::Finish()
{
  Log::Write("Finishing cabinet");

  m_pToys->Finish();
  m_pOutputControllers->Finish();
  Log::Write("Cabinet finished");
}

}  // namespace DOF
