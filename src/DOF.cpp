#include "DOF/DOF.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>

#include "DOF/Config.h"
#include "Log.h"
#include "Logger.h"
#include "Pinball.h"

namespace DOF
{

DOF::DOF() { }

DOF::~DOF() { }

void DOF::Init(const char* tableFilename, const char* romName)
{
   Config* pConfig = Config::GetInstance();
   std::string globalConfigPath = std::string(pConfig->GetBasePath()) + "directoutputconfig" + PATH_SEPARATOR_CHAR + "GlobalConfig_B2SServer.xml";
   if (std::filesystem::exists(globalConfigPath))
      Log::Write("Global configuration found at: %s", globalConfigPath.c_str());
   else
   {
      globalConfigPath = std::string(".") + PATH_SEPARATOR_CHAR + "directoutputconfig" + PATH_SEPARATOR_CHAR + "GlobalConfig_B2SServer.xml";
      if (std::filesystem::exists(globalConfigPath))
         Log::Write("Global configuration found at: %s", globalConfigPath.c_str());
      else
      {
         globalConfigPath = std::string(pConfig->GetBasePath()) + "directoutputconfig" + PATH_SEPARATOR_CHAR + "GlobalConfig_B2SServer.xml";
         Log::Warning("Unable to find global configuration. Defaulting to:  %s", globalConfigPath.c_str());
      }
   }

   Pinball* pPinball = Pinball::GetInstance();
   pPinball->Setup(globalConfigPath, tableFilename, romName);
   pPinball->Init();
}

void DOF::DataReceive(char type, int number, int value) { Pinball::GetInstance()->ReceiveData(type, number, value); }

void DOF::Finish() { Pinball::GetInstance()->Finish(); }

} // namespace DOF