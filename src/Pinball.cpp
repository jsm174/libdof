#include "Pinball.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>
#include <filesystem>

#include "Log.h"
#include "Logger.h"
#include "cab/Cabinet.h"
#include "cab/out/OutputControllerList.h"
#include "cab/toys/ToyList.h"
#include "globalconfiguration/GlobalConfig.h"
#include "pinballsupport/AlarmHandler.h"
#include "table/Table.h"

namespace DOF
{

Pinball* Pinball::m_pInstance = NULL;

Pinball* Pinball::GetInstance()
{
  if (!m_pInstance) m_pInstance = new Pinball();

  return m_pInstance;
}

Pinball::Pinball()
{
  m_pTable = new Table();
  m_pCabinet = new Cabinet();
  m_pAlarms = new AlarmHandler();
  m_pGlobalConfig = new GlobalConfig();
}

void Pinball::Setup(const std::string& szGlobalConfigFilename, const std::string& szTableFilename,
                    const std::string& szRomName)
{
  bool globalConfigLoaded = true;

  // Load the global config

  Log::Write("Global config filename is \"%s\"", szGlobalConfigFilename.c_str());
  if (!szGlobalConfigFilename.empty())
  {
    m_pGlobalConfig = GlobalConfig::GetGlobalConfigFromConfigXmlFile(szGlobalConfigFilename);
    if (!m_pGlobalConfig)
    {
      Log::Write("No global config file loaded");
      globalConfigLoaded = false;
      m_pGlobalConfig = new GlobalConfig();
    }
    m_pGlobalConfig->SetGlobalConfigFilename(szGlobalConfigFilename);
  }
  else
  {
    m_pGlobalConfig = new GlobalConfig();
    m_pGlobalConfig->SetGlobalConfigFilename(szGlobalConfigFilename);
  }

  if (m_pGlobalConfig->IsEnableLogging())
  {
    if (m_pGlobalConfig->IsClearLogOnSessionStart())
    {
      std::string szLogFilename = m_pGlobalConfig->GetLogFilename(szTableFilename, szRomName);
      if (!szLogFilename.empty())
      {
        if (std::filesystem::exists(szLogFilename))
        {
          try
          {
            std::filesystem::remove(szLogFilename);
          }
          catch (...)
          {
          }
        }
      }
      Log::SetFilename(m_pGlobalConfig->GetLogFilename(szTableFilename, szRomName));
      Log::Init();
    }
  }

  Log::AfterInit();

  if (globalConfigLoaded)
  {
    Log::Write("Global config file loaded from: %s", szGlobalConfigFilename.c_str());
  }
  else
  {
    if (!szGlobalConfigFilename.empty())
    {
      Log::Write("Could not find or load the global config file %s", szGlobalConfigFilename.c_str());
    }
    else
    {
      Log::Write("No GlobalConfig file loaded. Using newly instanciated GlobalConfig object instead.");
    }
  }

  Log::Write("Loading Pinball parts");
  Log::Write("Loading cabinet");

  m_pCabinet = NULL;
  std::string szCabinetConfigFile = m_pGlobalConfig->GetCabinetConfigFile();
  if (!szCabinetConfigFile.empty())
  {
    if (std::filesystem::exists(szCabinetConfigFile))
    {
      Log::Write("Will load cabinet config file: %s", szCabinetConfigFile.c_str());
      m_pCabinet = Cabinet::GetCabinetFromConfigXmlFile(szCabinetConfigFile);

      Log::Write("%d output controller definitions and %d toy definitions loaded from cabinet config.",
                 m_pCabinet->GetOutputControllers()->size(), m_pCabinet->GetToys()->size());

      m_pCabinet->SetCabinetConfigurationFilename(szCabinetConfigFile);
      if (m_pCabinet->IsAutoConfigEnabled())
      {
        Log::Write("Cabinet config file has AutoConfig feature enabled. Calling AutoConfig.");
        m_pCabinet->AutoConfig();
        Log::Write("Autoconfig complete.");
      }
      Log::Write("Cabinet config loaded successfully from %s", szCabinetConfigFile.c_str());
    }
    else
    {
      Log::Write("Cabinet config file %s does not exist.", szCabinetConfigFile.c_str());
    }
  }
  if (!m_pCabinet)
  {
    Log::Write("No cabinet config file loaded. Will use AutoConfig.");
    m_pCabinet = new Cabinet();
    m_pCabinet->AutoConfig();
  }

  Log::Write("Cabinet loaded");

  Log::Write("Loading table config");

  // Load table config

  m_pTable = new Table();
  m_pTable->SetAddLedControlConfig(true);

  if (!szTableFilename.empty())
  {
    std::string szTableConfigFile = m_pGlobalConfig->GetTableConfigFile(szTableFilename);
    if (!szTableConfigFile.empty())
    {
      if (std::filesystem::exists(szTableConfigFile))
      {
        Log::Write("Will load table config from: %s", szTableConfigFile.c_str());
        m_pTable = Table::GetTableFromConfigXmlFile(szTableConfigFile);
        m_pTable->SetTableConfigurationFilename(szTableConfigFile);
        Log::Write("Table config loaded successfully from %s", szTableConfigFile.c_str());
        if (m_pTable->IsAddLedControlConfig())
        {
          Log::Write("Table config allows mix with ledcontrol configs.");
        }
      }
      else
      {
        Log::Warning("No table config file found. Will try to load config from LedControl file(s).");
      }
    }
    else
    {
      Log::Write("No TableFilename specified, will use empty tableconfig");
    }
  }
  if (m_pTable->IsAddLedControlConfig())
  {
    if (!szRomName.empty())
    {
      Log::Write("Will try to load configs from DirectOutput.ini or LedControl.ini file(s) for RomName %s",
                 szRomName.c_str());

      std::map<int, std::string> iniFilesDictionary = m_pGlobalConfig->GetIniFilesDictionary(szTableFilename);
    }
  }
}

void Pinball::Init() {}

void Pinball::Finish() {}

void Pinball::ReceiveData(char type, int number, int value)
{
  Log::Write("Pinball::ReceiveData: type=%c, number=%d, value=%d", type, number, value);
}

}  // namespace DOF
