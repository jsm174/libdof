#include "DOF/DOF.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>

#include "DOF/Config.h"
#include "Log.h"
#include "Logger.h"
#include "Pinball.h"
#include "general/StringExtensions.h"

namespace DOF
{

DOF::DOF()
   : m_pinball(new Pinball())
{
}

DOF::~DOF() { delete m_pinball; }

void DOF::Init(const char* tableFilename, const char* romName)
{
   Config* config = Config::GetInstance();
   std::string globalConfigPath = std::string(config->GetBasePath()) + "directoutputconfig" + PATH_SEPARATOR_CHAR + "GlobalConfig_B2SServer.xml";
   if (std::filesystem::exists(globalConfigPath))
      Log::Write(StringExtensions::Build("Global configuration found at: {0}", globalConfigPath));
   else
   {
      globalConfigPath = std::string(".") + PATH_SEPARATOR_CHAR + "directoutputconfig" + PATH_SEPARATOR_CHAR + "GlobalConfig_B2SServer.xml";
      if (std::filesystem::exists(globalConfigPath))
         Log::Write(StringExtensions::Build("Global configuration found at: {0}", globalConfigPath));
      else
      {
         globalConfigPath = std::string(config->GetBasePath()) + "directoutputconfig" + PATH_SEPARATOR_CHAR + "GlobalConfig_B2SServer.xml";
         Log::Warning(StringExtensions::Build("Unable to find global configuration. Defaulting to: {0}", globalConfigPath));
      }
   }

   m_pinball->Setup(globalConfigPath, tableFilename, romName);
   m_pinball->Init();
}

void DOF::DataReceive(char type, int number, int value) { m_pinball->ReceiveData(type, number, value); }

void DOF::Finish() { m_pinball->Finish(); }

}