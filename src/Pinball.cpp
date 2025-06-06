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
#include "general/Utils.h"
#include "globalconfiguration/GlobalConfig.h"
#include "pinballsupport/AlarmHandler.h"
#include "table/Table.h"

namespace DOF
{

Pinball* Pinball::m_pInstance = NULL;

Pinball* Pinball::GetInstance()
{
   if (!m_pInstance)
      m_pInstance = new Pinball();

   return m_pInstance;
}

Pinball::Pinball()
{
   m_pTable = new Table();
   m_pCabinet = new Cabinet();
   m_pAlarms = new AlarmHandler();
   m_pGlobalConfig = new GlobalConfig();
}

void Pinball::Setup(const std::string& globalConfigFileName, const std::string& tableFilename, const std::string& romName)
{
   bool globalConfigLoaded = true;
   // Load the global config

   try
   {
      Log::Write("Global config filename is \"%s\"", globalConfigFileName.c_str());
      if (!globalConfigFileName.empty())
      {
         FileInfo* pGlobalConfigFile = new FileInfo(globalConfigFileName);
         m_pGlobalConfig = GlobalConfig::GetGlobalConfigFromConfigXmlFile(pGlobalConfigFile->FullName());
         if (!m_pGlobalConfig)
         {
            Log::Write("No global config file loaded");
            globalConfigLoaded = false;

            // set new global config object if it config could not be loaded from the file.
            m_pGlobalConfig = new GlobalConfig();
         }
         m_pGlobalConfig->SetGlobalConfigFilename(globalConfigFileName);
      }
      else
      {
         m_pGlobalConfig = new GlobalConfig();
         m_pGlobalConfig->SetGlobalConfigFilename(globalConfigFileName);
      }
   }
   catch (...)
   {
      throw std::runtime_error("DirectOutput framework could not initialize global config.");
   }

   if (m_pGlobalConfig->IsEnableLogging())
   {
      if (m_pGlobalConfig->IsClearLogOnSessionStart())
      {
         try
         {
            std::string tablePath = !IsNullOrWhiteSpace(tableFilename) ? FileInfo(tableFilename).FullName() : "";

            FileInfo* pLF = new FileInfo(m_pGlobalConfig->GetLogFilename(tablePath, romName));
            if (pLF->Exists())
            {
               pLF->Delete();
            }
         }
         catch (...)
         {
         }
      }
      try
      {
         std::string tablePath = !IsNullOrWhiteSpace(tableFilename) ? FileInfo(tableFilename).FullName() : "";
         std::string logFilename = m_pGlobalConfig->GetLogFilename(tablePath, romName);
         Log::SetFilename(m_pGlobalConfig->GetLogFilename(tableFilename, romName));
         Log::Init();
      }
      catch (...)
      {
         throw std::runtime_error("DirectOutput framework could initialize the log file.");
      }
   }

   // finalize logger initialization
   Log::AfterInit();

   try
   {
      if (globalConfigLoaded)
      {
         Log::Write("Global config file loaded from: %s", globalConfigFileName.c_str());
      }
      else
      {
         if (!IsNullOrWhiteSpace(globalConfigFileName))
         {
            Log::Write("Could not find or load the global config file %s", globalConfigFileName.c_str());
         }
         else
         {
            Log::Write("No GlobalConfig file loaded. Using newly instanciated GlobalConfig object instead.");
         }
      }

      Log::Write("Loading Pinball parts");

      Log::Write("Loading cabinet");
      // Load cabinet config
      m_pCabinet = NULL;
      auto ccf = m_pGlobalConfig->GetCabinetConfigFile();
      if (ccf != nullptr)
      {
         if (ccf->Exists())
         {
            Log::Write("Will load cabinet config file: %s", ccf->FullName().c_str());

            try
            {
               m_pCabinet = Cabinet::GetCabinetFromConfigXmlFile(ccf);

               Log::Write("%d output controller definitions and %d toy definitions loaded from cabinet config.", m_pCabinet->GetOutputControllers()->size(), m_pCabinet->GetToys()->size());

               m_pCabinet->SetCabinetConfigurationFilename(ccf->FullName());
               if (m_pCabinet->IsAutoConfigEnabled())
               {
                  Log::Write("Cabinet config file has AutoConfig feature enabled. Calling AutoConfig.");
                  try
                  {
                     m_pCabinet->AutoConfig();
                  }
                  catch (...)
                  {
                     Log::Exception("A exeption occured during cabinet auto configuration");
                  }
                  Log::Write("Autoconfig complete.");
               }
               Log::Write("Cabinet config loaded successfully from %s", ccf->FullName().c_str());
            }
            catch (...)
            {
               Log::Exception("An exception occured when loading cabinet config file: %s", ccf->FullName().c_str());
            }
         }
         else
         {
            Log::Warning("Cabinet config file %s does not exist.", ccf->FullName().c_str());
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

      if (!IsNullOrWhiteSpace(tableFilename))
      {
         FileInfo* pTableFile = new FileInfo(tableFilename);
         FileInfo* pTCF = m_pGlobalConfig->GetTableConfigFile(pTableFile->FullName());
         if (pTCF != nullptr)
         {
            Log::Write("Will load table config from %s", pTCF->FullName().c_str());
            try
            {
               m_pTable = Table::GetTableFromConfigXmlFile(pTableFile->FullName());
               m_pTable->SetTableConfigurationFilename(m_pGlobalConfig->GetTableConfigFile(pTableFile->FullName())->FullName());
               Log::Write("Table config loaded successfully from %s", pTCF->FullName().c_str());
            }
            catch (...)
            {
               Log::Exception("A exception occured when loading table config: %s", pTCF->FullName().c_str());
            }
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
      if (m_pTable->IsAddLedControlConfig())
      {
         if (!romName.empty())
         {
            Log::Write("Will try to load configs from DirectOutput.ini or LedControl.ini file(s) for RomName %s", romName.c_str());

            //std::map<int, std::string> iniFilesDictionary = m_pGlobalConfig->GetIniFilesDictionary(tableFilename);
         }
      }
      if (!IsNullOrWhiteSpace(m_pTable->GetTableName()))
      {
         if (!IsNullOrWhiteSpace(tableFilename))
         {
            // TODO: m_pTable->SetTableName(Path.GetFileNameWithoutExtension(new FileInfo(TableFilename).FullName));
         }
         else if (!IsNullOrWhiteSpace(romName))
         {
            m_pTable->SetTableName(romName);
         }
      }
      if (!IsNullOrWhiteSpace(tableFilename))
      {
         m_pTable->SetTableFilename(FileInfo(tableFilename).FullName());
      }
      if (!IsNullOrWhiteSpace(romName))
      {
         m_pTable->SetRomName(romName);
      }
      Log::Write("Table config loading finished: romname=%s, tablename=%s", romName.c_str(), m_pTable->GetTableName().c_str());

      // update table overrider with romname and tablename references, and activate valid overrides
      // TODO:TableOverrideSettings.Instance.activeromName = RomName;
      // TODO:TableOverrideSettings.Instance.activetableName = Table.TableName;
      // TODO:TableOverrideSettings.Instance.activateOverrides();

      Log::Write("Pinball parts loaded");
   }
   catch (...)
   {
      Log::Exception("DirectOutput framework has encountered a exception during setup.");
      throw std::runtime_error("DirectOutput framework has encountered a exception during setup.");
   }
}

void Pinball::Init() { }

void Pinball::Finish() { }

void Pinball::ReceiveData(char type, int number, int value) { Log::Debug("Pinball::ReceiveData: type=%c, number=%d, value=%d", type, number, value); }

} // namespace DOF
