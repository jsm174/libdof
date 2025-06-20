#include "Pinball.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>
#include <filesystem>

#include "Log.h"
#include "Logger.h"
#include "cab/Cabinet.h"
#include "cab/ICabinetOwner.h"
#include "cab/CabinetOwner.h"
#include "cab/out/OutputControllerList.h"
#include "cab/overrides/TableOverrideSettings.h"
#include "cab/toys/ToyList.h"
#include "globalconfiguration/GlobalConfig.h"
#include "pinballsupport/AlarmHandler.h"
#include "pinballsupport/InputQueue.h"
#include "table/Table.h"
#include "table/TableElementData.h"
#include "ledcontrol/loader/LedControlConfigList.h"
#include "ledcontrol/loader/LedControlConfig.h"
#include "ledcontrol/setup/Configurator.h"
#include "general/StringExtensions.h"

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
   : m_pTable(new Table())
   , m_pCabinet(new Cabinet())
   , m_pAlarms(new AlarmHandler())
   , m_pGlobalConfig(new GlobalConfig())
   , m_pInputQueue(new InputQueue())
   , m_keepMainThreadAlive(false)
   , m_mainThreadDoWork(false)
{
}

void Pinball::Setup(const std::string& globalConfigFileName, const std::string& tableFilename, const std::string& romName)
{
   bool globalConfigLoaded = true;

   try
   {
      Log::Write(StringExtensions::Build("Global config filename is \"{0}\"", globalConfigFileName));
      if (!globalConfigFileName.empty())
      {
         FileInfo* pGlobalConfigFile = new FileInfo(globalConfigFileName);
         m_pGlobalConfig = GlobalConfig::GetGlobalConfigFromConfigXmlFile(pGlobalConfigFile->FullName());
         if (!m_pGlobalConfig)
         {
            Log::Write("No global config file loaded");
            globalConfigLoaded = false;

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
   catch (const std::exception& e)
   {
      throw std::runtime_error("DirectOutput framework could not initialize global config.");
   }

   if (m_pGlobalConfig->IsEnableLogging())
   {
      if (m_pGlobalConfig->IsClearLogOnSessionStart())
      {
         try
         {
            FileInfo* pLF = new FileInfo(m_pGlobalConfig->GetLogFilename(!StringExtensions::IsNullOrWhiteSpace(tableFilename) ? FileInfo(tableFilename).FullName() : "", romName));
            if (pLF->Exists())
               pLF->Delete();
         }
         catch (const std::exception& e)
         {
         }
      }
      try
      {
         Log::SetFilename(m_pGlobalConfig->GetLogFilename(!StringExtensions::IsNullOrWhiteSpace(tableFilename) ? FileInfo(tableFilename).FullName() : "", romName));
         Log::Init();
      }
      catch (const std::exception& e)
      {
         throw std::runtime_error("DirectOutput framework could initialize the log file.");
      }
   }

   Log::AfterInit();

   try
   {
      if (globalConfigLoaded)
      {
         Log::Write(StringExtensions::Build("Global config file loaded from: {0}", globalConfigFileName));
      }
      else
      {
         if (!StringExtensions::IsNullOrWhiteSpace(globalConfigFileName))
         {
            Log::Write(StringExtensions::Build("Could not find or load the global config file {0}", globalConfigFileName));
         }
         else
         {
            Log::Write("No GlobalConfig file loaded. Using newly instanciated GlobalConfig object instead.");
         }
      }

      Log::Write("Loading Pinball parts");

      Log::Write("Loading cabinet");

      m_pCabinet = nullptr;
      FileInfo* pCCF = m_pGlobalConfig->GetCabinetConfigFile();
      if (pCCF != nullptr)
      {
         if (pCCF->Exists())
         {
            Log::Write(StringExtensions::Build("Will load cabinet config file: {0}", pCCF->FullName()));
            try
            {
               m_pCabinet = Cabinet::GetCabinetFromConfigXmlFile(pCCF);

               Log::Write(StringExtensions::Build("{0} output controller definitions and {1} toy definitions loaded from cabinet config.",
                  std::to_string(static_cast<int>(m_pCabinet->GetOutputControllers()->size())), std::to_string(static_cast<int>(m_pCabinet->GetToys()->size()))));

               m_pCabinet->SetCabinetConfigurationFilename(pCCF->FullName());
               if (m_pCabinet->IsAutoConfigEnabled())
               {
                  Log::Write("Cabinet config file has AutoConfig feature enabled. Calling AutoConfig.");
                  try
                  {
                     m_pCabinet->AutoConfig();
                  }
                  catch (const std::exception& e)
                  {
                     Log::Exception(StringExtensions::Build("An exception occured during cabinet auto configuration: {0}", e.what()));
                  }
                  Log::Write("Autoconfig complete.");
               }
               Log::Write(StringExtensions::Build("Cabinet config loaded successfully from {0}", pCCF->FullName()));
            }
            catch (const std::exception& e)
            {
               Log::Exception(StringExtensions::Build("An exception occured when loading cabinet config file: {0} - {1}", pCCF->FullName(), e.what()));
            }
         }
         else
         {
            Log::Warning(StringExtensions::Build("Cabinet config file {0} does not exist.", pCCF->FullName()));
         }
      }
      if (m_pCabinet == nullptr)
      {
         Log::Write("No cabinet config file loaded. Will use AutoConfig.");

         m_pCabinet = new Cabinet();
         m_pCabinet->AutoConfig();
      }

      Log::Write("Cabinet loaded");

      Log::Write("Loading table config");

      m_pTable = new Table();
      m_pTable->SetAddLedControlConfig(true);

      if (!StringExtensions::IsNullOrWhiteSpace(tableFilename))
      {
         FileInfo* pTableFile = new FileInfo(tableFilename);
         FileInfo* pTCF = m_pGlobalConfig->GetTableConfigFile(pTableFile->FullName());
         if (pTCF != nullptr)
         {
            Log::Write(StringExtensions::Build("Will load table config from {0}", pTCF->FullName()));
            try
            {
               m_pTable = Table::GetTableFromConfigXmlFile(pTableFile->FullName());
               m_pTable->SetTableConfigurationFilename(m_pGlobalConfig->GetTableConfigFile(pTableFile->FullName())->FullName());
               Log::Write(StringExtensions::Build("Table config loaded successfully from {0}", pTCF->FullName()));
            }
            catch (const std::exception& e)
            {
               Log::Exception(StringExtensions::Build("An exception occured when loading table config: {0} - {1}", pTCF->FullName(), e.what()));
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
         if (!StringExtensions::IsNullOrWhiteSpace(romName))
         {
            Log::Write(StringExtensions::Build("Will try to load configs from DirectOutput.ini or LedControl.ini file(s) for RomName {0}", romName));

            std::unordered_map<int, FileInfo> ledControlIniFiles = m_pGlobalConfig->GetIniFilesDictionary(tableFilename);

            LedControlConfigList* l = new LedControlConfigList();
            if (ledControlIniFiles.size() > 0)
            {
               l->LoadLedControlFiles(ledControlIniFiles, false);
               Log::Write(StringExtensions::Build("{0} directoutputconfig.ini or ledcontrol.ini files loaded.", std::to_string(ledControlIniFiles.size())));
            }
            else
            {
               Log::Write("No directoutputconfig.ini or ledcontrol.ini files found.");
            }

            if (!l->ContainsConfig(romName))
            {
               Log::Write(StringExtensions::Build("No config for table found in LedControl data for RomName {0}.", romName));
            }
            else
            {
               Log::Write(StringExtensions::Build("Config for RomName {0} exists in LedControl data. Updating cabinet and config.", romName));

               Configurator* pConfigurator = new Configurator();
               pConfigurator->SetEffectMinDurationMs(m_pGlobalConfig->GetLedControlMinimumEffectDurationMs());
               pConfigurator->SetEffectRGBMinDurationMs(m_pGlobalConfig->GetLedControlMinimumRGBEffectDurationMs());
               pConfigurator->Setup(l, m_pTable, m_pCabinet, romName);
               delete pConfigurator;

               std::string dofVersion = LIBDOF_VERSION;

               if (true)
               {
                  std::string maxVersion = "";
                  for (auto* pLedConfig : *l)
                  {
                     if (pLedConfig && pLedConfig->HasMinDOFVersion())
                     {
                     }
                  }

                  Log::Warning("UPDATE DIRECT OUTPUT FRAMEWORK!");
                  if (!maxVersion.empty())
                  {
                     Log::Warning(StringExtensions::Build("Current DOF version is {0}, but DOF version {1} or later is required by one or several config files.", dofVersion, maxVersion));
                  }
                  try
                  {
                  }
                  catch (const std::exception& e)
                  {
                     Log::Exception(StringExtensions::Build("An exception occured when displaying the update notification: {0}", e.what()));
                  }
               }
            }
            delete l;
         }
         else
         {
            Log::Write("Cant load config from directoutput.ini or ledcontrol.ini file(s) since no RomName was supplied. No ledcontrol config will be loaded.");
         }
      }
      if (StringExtensions::IsNullOrWhiteSpace(m_pTable->GetTableName()))
      {
         if (!StringExtensions::IsNullOrWhiteSpace(tableFilename))
            m_pTable->SetTableName(StringExtensions::GetFileNameWithoutExtension(FileInfo(tableFilename).FullName()));
         else if (!StringExtensions::IsNullOrWhiteSpace(romName))
            m_pTable->SetTableName(romName);
      }
      if (!StringExtensions::IsNullOrWhiteSpace(tableFilename))
         m_pTable->SetTableFilename(FileInfo(tableFilename).FullName());
      if (!StringExtensions::IsNullOrWhiteSpace(romName))
         m_pTable->SetRomName(romName);
      Log::Write(StringExtensions::Build("Table config loading finished: romname={0}, tablename={1}", romName, m_pTable->GetTableName()));

      TableOverrideSettings::GetInstance()->SetActiveRomName(romName);
      TableOverrideSettings::GetInstance()->SetActiveTableName(m_pTable->GetTableName());
      TableOverrideSettings::GetInstance()->ActivateOverrides();

      Log::Write("Pinball parts loaded");
   }
   catch (const std::exception& e)
   {
      Log::Exception(StringExtensions::Build("DirectOutput framework has encountered an exception during setup: {0}", e.what()));
      throw std::runtime_error("DirectOutput framework has encountered a exception during setup.");
   }
}

void Pinball::Init()
{
   try
   {
      Log::Write("Starting processes");

      CabinetOwner* pCO = new CabinetOwner();
      pCO->SetAlarms(m_pAlarms);
      pCO->GetConfigurationSettings().emplace("LedControlMinimumEffectDurationMs", std::to_string(m_pGlobalConfig->GetLedControlMinimumEffectDurationMs()));
      pCO->GetConfigurationSettings().emplace("LedWizDefaultMinCommandIntervalMs", std::to_string(m_pGlobalConfig->GetLedWizDefaultMinCommandIntervalMs()));
      pCO->GetConfigurationSettings().emplace("PacLedDefaultMinCommandIntervalMs", std::to_string(m_pGlobalConfig->GetPacLedDefaultMinCommandIntervalMs()));
      m_pCabinet->Init(pCO);

      m_pTable->Init(this);
      m_pAlarms->Init(this);
      m_pTable->TriggerStaticEffects();
      m_pCabinet->Update();

      InitMainThread();
      Log::Write("Framework initialized.");
      Log::Write("Have fun! :)");
   }
   catch (const std::exception& e)
   {
      Log::Exception(StringExtensions::Build("DirectOutput framework has encountered an exception during initialization: {0}", e.what()));
      throw std::runtime_error("DirectOutput framework has encountered a exception during initialization.");
   }
}

void Pinball::Finish()
{
   try
   {
      Log::Write("Finishing framework");
      FinishMainThread();

      m_pAlarms->Finish();
      m_pTable->Finish();
      m_pCabinet->Finish();

      Log::Write("DirectOutput framework finished.");
      Log::Write("Bye and thanks for using!");
   }
   catch (const std::exception& e)
   {
      Log::Exception(StringExtensions::Build("An exception occured while finishing the DirectOutput framework: {0}", e.what()));
      throw std::runtime_error("DirectOutput framework has encountered while finishing.");
   }
}

void Pinball::InitMainThread()
{
   if (!IsMainThreadActive())
   {
      m_keepMainThreadAlive = true;
      try
      {
         m_mainThread = std::thread(&Pinball::MainThreadDoIt, this);
      }
      catch (const std::exception& e)
      {
         Log::Exception(StringExtensions::Build("DirectOutput MainThread could not start: {0}", e.what()));
         throw std::runtime_error("DirectOutput MainThread could not start.");
      }
   }
}

void Pinball::FinishMainThread()
{
   if (m_mainThread.joinable())
   {
      try
      {
         m_keepMainThreadAlive = false;
         m_mainThreadCV.notify_one();

         if (m_mainThread.joinable())
         {
            m_mainThread.join();
         }
      }
      catch (const std::exception& e)
      {
         Log::Exception(StringExtensions::Build("An error occured during termination of DirectOutput MainThread: {0}", e.what()));
         throw std::runtime_error("An error occured during termination of DirectOutput MainThread");
      }
   }
}

void Pinball::MainThreadSignal()
{
   std::lock_guard<std::mutex> lock(m_mainThreadMutex);
   m_mainThreadDoWork = true;
   m_mainThreadCV.notify_one();
}

void Pinball::MainThreadDoIt()
{
   try
   {
      while (m_keepMainThreadAlive)
      {
         bool updateRequired = false;
         auto start = std::chrono::steady_clock::now();

         while (m_pInputQueue->Count() > 0 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() <= MAX_INPUT_DATA_PROCESSING_TIME_MS
            && m_keepMainThreadAlive)
         {
            try
            {
               TableElementData data = m_pInputQueue->Dequeue();
               m_pTable->UpdateTableElement(&data);
               updateRequired = true;
            }
            catch (const std::exception& e)
            {
               Log::Exception(StringExtensions::Build("An unhandled exception occured while processing table element data: {0}", e.what()));
            }
         }

         if (m_keepMainThreadAlive)
         {
            try
            {

               auto now = std::chrono::steady_clock::now();
               updateRequired |= m_pAlarms->ExecuteAlarms(now + std::chrono::milliseconds(1));
            }
            catch (const std::exception& e)
            {
               Log::Exception(StringExtensions::Build("An unhandled exception occured while executing timer events: {0}", e.what()));
            }
         }

         if (updateRequired && m_keepMainThreadAlive)
         {
            try
            {
               m_pCabinet->Update();
            }
            catch (const std::exception& e)
            {
               Log::Exception(StringExtensions::Build("An unhandled exception occured while updating the output controllers: {0}", e.what()));
            }
         }

         if (m_keepMainThreadAlive)
         {

            auto nextAlarm = m_pAlarms->GetNextAlarmTime();
            auto now = std::chrono::steady_clock::now();

            std::unique_lock<std::mutex> lock(m_mainThreadMutex);

            while (m_pInputQueue->Count() == 0 && nextAlarm > now && !m_mainThreadDoWork && m_keepMainThreadAlive)
            {
               long timeoutMs = std::chrono::duration_cast<std::chrono::milliseconds>(nextAlarm - now).count();
               timeoutMs = std::max(1L, std::min(timeoutMs, 50L));

               m_mainThreadCV.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this]() { return m_pInputQueue->Count() > 0 || m_mainThreadDoWork || !m_keepMainThreadAlive; });

               now = std::chrono::steady_clock::now();
               nextAlarm = m_pAlarms->GetNextAlarmTime();
            }
            m_mainThreadDoWork = false;
         }
      }
   }
   catch (const std::exception& e)
   {
      Log::Exception(StringExtensions::Build("An unexpected exception occured in the DirectOutput MainThread: {0}", e.what()));
   }
}

void Pinball::ReceiveData(char type, int number, int value)
{
   m_pInputQueue->Enqueue(type, number, value);
   MainThreadSignal();
}

void Pinball::ReceiveData(const std::string& tableElementName, int value)
{
   m_pInputQueue->Enqueue(tableElementName, value);
   MainThreadSignal();
}

void Pinball::ReceiveData(const TableElementData& tableElementData)
{
   m_pInputQueue->Enqueue(tableElementData);
   MainThreadSignal();
}

}
