#include "GlobalConfig.h"

#include <filesystem>

#include "../Log.h"

namespace DOF
{

GlobalConfig::GlobalConfig()
{
  m_ledWizDefaultMinCommandIntervalMs = 10;
  m_ledControlMinimumEffectDurationMs = 60;
  m_ledControlMinimumRGBEffectDurationMs = 120;
  m_pacLedDefaultMinCommandIntervalMs = 10;
  m_enableLog = true;
  m_clearLogOnSessionStart = true;
}

void GlobalConfig::SetLedWizDefaultMinCommandIntervalMs(int value)
{
  m_ledWizDefaultMinCommandIntervalMs = std::clamp(value, 0, 1000);
}

void GlobalConfig::SetPacLedDefaultMinCommandIntervalMs(int value)
{
  m_pacLedDefaultMinCommandIntervalMs = std::clamp(value, 0, 1000);
}

std::map<int, std::string> GlobalConfig::GetIniFilesDictionary(const std::string& szTableFilename)
{
  std::vector<std::string> lookupPaths;

  Config* pConfig = Config::GetInstance();
  std::string szBasePath = pConfig->GetBasePath();

  if (!szBasePath.empty()) lookupPaths.push_back(szBasePath);

  if (!m_szIniFilesPath.empty()) lookupPaths.push_back(m_szIniFilesPath);

  if (!szTableFilename.empty())
  {
    if (std::filesystem::exists(szTableFilename))
    {
      std::filesystem::path path(szTableFilename);
      lookupPaths.push_back(path.parent_path().string());
    }
  }

  if (!GetGlobalConfigDirectory().empty()) lookupPaths.push_back(GetGlobalConfigDirectory());

  lookupPaths.push_back(std::filesystem::current_path().string());

  // Build the dictionary of ini files

  std::map<int, std::string> iniFilesDictionary;

  bool foundIt = false;
  const std::string ledControlFilenames[] = {"directoutputconfig", "ledcontrol"};

  for (const auto& szLedControlFilename : ledControlFilenames)
  {
    for (const auto& szLookupPath : lookupPaths)
    {
      std::vector<std::string> files;
      for (const auto& entry : std::filesystem::directory_iterator(szLookupPath))
      {
        if (entry.is_regular_file())
        {
          std::string filename = entry.path().filename().string();
          std::transform(filename.begin(), filename.end(), filename.begin(),
                         [](unsigned char c) { return std::tolower(c); });

          if (filename.starts_with(szLedControlFilename) && filename.ends_with(".ini"))
          {
            files.push_back(entry.path().string());
          }
        }
      }

      for (const auto& file : files)
      {
        std::string filename = file;

        std::transform(filename.begin(), filename.end(), filename.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        std::string expectedName = szLedControlFilename + ".ini";
        std::transform(expectedName.begin(), expectedName.end(), expectedName.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (filename == expectedName)
        {
          if (!iniFilesDictionary.contains(1))
          {
            iniFilesDictionary[1] = file;
            foundIt = true;
            break;
          }
          else
          {
            Log::Write("Found more than one ini file for number 1.");
          }
        }
        else
        {
          std::string numStr =
              filename.substr(szLedControlFilename.length(), filename.length() - szLedControlFilename.length() - 4);
          if (std::all_of(numStr.begin(), numStr.end(), ::isdigit))
          {
            int ledWizNr = std::stoi(numStr);
            if (!iniFilesDictionary.contains(ledWizNr))
            {
              iniFilesDictionary[ledWizNr] = file;
              foundIt = true;
            }
            else
            {
              Log::Write("Found more than one ini file with number %d.", ledWizNr);
            }
          }
        }
      }

      if (foundIt) break;
    }

    if (foundIt) break;
  }

  return iniFilesDictionary;
}

std::string GlobalConfig::GetTableMappingFile(const std::string& szTableFilename)
{
  Log::Warning("Not implemented");
  return "";
}

std::string GlobalConfig::GetShapeDefinitionFile(const std::string& szTableFilename, const std::string& szRomName)
{
  Log::Warning("Not implemented");
  return "";
}

std::string GlobalConfig::GetCabinetConfigFile()
{
  Log::Warning("Not implemented");
  return "";
}

std::string GlobalConfig::GetCabinetConfigDirectory()
{
  Log::Warning("Not implemented");
  return "";
}

std::string GlobalConfig::GetTableConfigFile(const std::string& szTableFilename)
{
  Log::Warning("Not implemented");
  return "";
}

std::string GlobalConfig::GetLogFilename(const std::string& szTableFilename, const std::string& szRomName)
{
  Log::Warning("Not implemented");
  return "";
}

std::map<std::string, std::string> GlobalConfig::GetReplaceValuesDictionary(const std::string& szTableFilename,
                                                                            const std::string& szRomName)
{
  Log::Warning("Not implemented");
  return {};
}

std::string GlobalConfig::GetGlobalConfigDirectoryName()
{
  Log::Warning("Not implemented");
  return {};
}

std::string GlobalConfig::GetGlobalConfigDirectory()
{
  Log::Warning("Not implemented");
  return {};
}

std::string GlobalConfig::GetGlobalConfigFile()
{
  Log::Warning("Not implemented");
  return {};
}

std::string GlobalConfig::GetGlobalConfigXml()
{
  Log::Warning("Not implemented");
  return {};
}

GlobalConfig* GlobalConfig::GetGlobalConfigFromConfigXmlFile(const std::string& szGlobalConfigFileName)
{
  Log::Warning("Not implemented");
  return nullptr;
}

GlobalConfig* GlobalConfig::GetGlobalConfigFromGlobalConfigXml(const std::string& szConfigXml)
{
  Log::Warning("Not implemented");
  return nullptr;
}

void GlobalConfig::SaveGlobalConfig(const std::string& szFilename) { Log::Warning("Not implemented"); }

}  // namespace DOF
