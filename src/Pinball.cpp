#include "Pinball.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>

#include "Logger.h"
#include "cab/Cabinet.h"
#include "globalconfiguration/GlobalConfiguration.h"
#include "table/Table.h"

namespace DOF
{

Pinball* Pinball::m_pInstance = NULL;

Pinball* Pinball::GetInstance()
{
  if (!m_pInstance) m_pInstance = new Pinball();

  return m_pInstance;
}

void Pinball::Setup(const char* szTableFilename, const char* szRomName)
{
  Log("Loading Pinball parts");
  Log("Loading cabinet");

  m_pCabinet = nullptr;

  if (GlobalConfiguration::GetInstance()->GetCabinetConfigFile())
  {
  }
  else
  {
    Log("Cabinet config file %s does not exist.", "cabinet.xml");
  }

  if (!m_pCabinet)
  {
    Log("No cabinet config file loaded. Will use AutoConfig.");
    m_pCabinet = new Cabinet();
    m_pCabinet->AutoConfig();
  }

  Log("Cabinet loaded");

  Log("Loading table config");
}

void Pinball::Init() {}

void Pinball::Finish() {}

void Pinball::ReceiveData(char type, int number, int value)
{
  Log("Pinball::ReceiveData: type=%c, number=%d, value=%d", type, number, value);
}

}  // namespace DOF
