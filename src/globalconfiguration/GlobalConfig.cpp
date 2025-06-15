#include "GlobalConfig.h"

#include "../Log.h"
#include "../general/FileReader.h"
#include "../general/DirectoryInfo.h"
#include "../general/StringExtensions.h"

#include <tinyxml2/tinyxml2.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

using namespace tinyxml2;

namespace DOF
{

GlobalConfig::GlobalConfig()
   : m_ledWizDefaultMinCommandIntervalMs(10)
   , m_ledControlMinimumEffectDurationMs(60)
   , m_ledControlMinimumRGBEffectDurationMs(120)
   , m_pacLedDefaultMinCommandIntervalMs(10)
   , m_enableLog(true)
   , m_clearLogOnSessionStart(true)
{
   m_tableConfigFilePatterns.clear();
   m_logFilePattern.SetPattern("./DirectOutput.log");
}

void GlobalConfig::SetLedWizDefaultMinCommandIntervalMs(int value) { m_ledWizDefaultMinCommandIntervalMs = std::clamp(value, 0, 1000); }

void GlobalConfig::SetPacLedDefaultMinCommandIntervalMs(int value) { m_pacLedDefaultMinCommandIntervalMs = std::clamp(value, 0, 1000); }

std::unordered_map<int, FileInfo> GlobalConfig::GetIniFilesDictionary(const std::string& tableFilename) const
{


   std::vector<std::string> lookupPaths;

   if (!StringExtensions::IsNullOrWhiteSpace(m_iniFilesPath))
   {
      try
      {
         DirectoryInfo di(m_iniFilesPath);
         if (di.Exists())
            lookupPaths.push_back(di.FullName());
      }
      catch (...)
      {
         Log::Exception(StringExtensions::Build("The specified IniFilesPath {0} could not be used due to an exception.", m_iniFilesPath));
      }
   }

   if (!StringExtensions::IsNullOrWhiteSpace(tableFilename))
   {
      try
      {
         if (FileInfo(tableFilename).Directory()->Exists())
            lookupPaths.push_back(FileInfo(tableFilename).Directory()->FullName());
      }

      catch (...)
      {
      }
   }

   if (GetGlobalConfigDirectory() != nullptr)
   {
      lookupPaths.push_back(GetGlobalConfigDirectory()->FullName());
   }

   lookupPaths.push_back(std::filesystem::current_path().string());


   std::unordered_map<int, FileInfo> iniFiles;

   bool foundIt = false;
   const std::string ledControlFilenames[] = { "directoutputconfig", "ledcontrol" };

   for (const std::string& ledControlFilename : ledControlFilenames)
   {
      for (const std::string& p : lookupPaths)
      {
         DirectoryInfo di(p);

         std::vector<FileInfo> files;
         for (FileInfo fi : di.EnumerateFiles())
         {
            if (StringExtensions::ToLower(fi.Name()).starts_with(ledControlFilename) && StringExtensions::ToLower(fi.Name()).ends_with(".ini"))
               files.push_back(fi);
         }

         for (FileInfo fi : files)
         {
            if (StringExtensions::ToLower(fi.Name() + ".ini") == StringExtensions::ToLower(ledControlFilename))
            {
               if (!iniFiles.contains(1))
               {
                  iniFiles.emplace(1, fi);
                  foundIt = true;
               }
               else
               {
                  Log::Warning("Found more than one ini file with for number 1. Likely you have a ini file without a number and and a second one with number 1.");
               }
            }
            else
            {
               std::string f = fi.Name().substr(ledControlFilename.length(), fi.Name().length() - ledControlFilename.length() - 4);
               if (StringExtensions::IsInteger(f))
               {
                  int ledWizNr = -1;
                  if (StringExtensions::TryParseInt(f, ledWizNr))
                  {
                     if (!iniFiles.contains(ledWizNr))
                     {
                        iniFiles.emplace(ledWizNr, fi);
                        foundIt = true;
                     }
                     else
                     {
                        Log::Warning(StringExtensions::Build("Found more than one ini file with number {0}.", std::to_string(ledWizNr)));
                     }
                  }
               }
            }
         }
         if (foundIt)
            break;
      }
      if (foundIt)
         break;
   }

   return iniFiles;
}

FileInfo* GlobalConfig::GetTableMappingFile(const std::string& tableFilename) const
{
   std::unordered_map<int, FileInfo> iniFilesDict = GetIniFilesDictionary(tableFilename);

   if (iniFilesDict.count(1) > 0)
   {
      auto [firstKey, firstValue] = *iniFilesDict.begin();
      return new FileInfo(firstValue.Directory()->GetFiles("tablemappings.*").front());
   }
   else
   {
      return nullptr;
   }
}

FileInfo* GlobalConfig::GetShapeDefinitionFile(const std::string& tableFilename, const std::string& romName) const
{
   if (!StringExtensions::IsNullOrWhiteSpace(m_shapeDefinitionFilePattern.GetPattern()) && m_shapeDefinitionFilePattern.IsValid())
      return m_shapeDefinitionFilePattern.GetFirstMatchingFile(GetReplaceValuesDictionary(tableFilename, romName));

   std::unordered_map<int, FileInfo> iniFilesDict = GetIniFilesDictionary(tableFilename);
   if (iniFilesDict.size() > 0)
   {
      auto [firstKey, firstValue] = *iniFilesDict.begin();
      FileInfo* pFI = new FileInfo(firstValue.Directory()->FullName() + PATH_SEPARATOR_CHAR + "DirectOutputShapes.xml");
      if (pFI->Exists())
         return pFI;
   }
   FileInfo* pFII = FilePattern(std::string("{DllDir}") + PATH_SEPARATOR_CHAR + "DirectOutputShapes.xml").GetFirstMatchingFile(GetReplaceValuesDictionary(tableFilename, romName));
   if (pFII != nullptr && pFII->Exists())
      return pFII;

   return nullptr;
}

FileInfo* GlobalConfig::GetCabinetConfigFile() const
{
   if (!StringExtensions::IsNullOrWhiteSpace(m_cabinetConfigFilePattern.GetPattern()) && m_cabinetConfigFilePattern.IsValid())
      return m_cabinetConfigFilePattern.GetFirstMatchingFile(GetReplaceValuesDictionary());
   return nullptr;
}

DirectoryInfo* GlobalConfig::GetCabinetConfigDirectory() const
{
   FileInfo* pCC = GetCabinetConfigFile();
   if (pCC != nullptr)
      return pCC->Directory();
   return nullptr;
}

FileInfo* GlobalConfig::GetTableConfigFile(const std::string& fullTableFilename) const
{
   return m_tableConfigFilePatterns.GetFirstMatchingFile(GetReplaceValuesDictionary(fullTableFilename));
}

std::string GlobalConfig::GetLogFilename(const std::string& tableFilename, const std::string& romName) const
{
   std::unordered_map<std::string, std::string> r = GetReplaceValuesDictionary(tableFilename, romName);
   auto now = std::chrono::system_clock::now();
   std::time_t t = std::chrono::system_clock::to_time_t(now);
   std::tm tm;
#if defined(_WIN32)
   localtime_s(&tm, &t);
#else
   localtime_r(&t, &tm);
#endif

   char buf[32];
   std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tm);
   r.emplace("DateTime", std::string(buf));
   std::strftime(buf, sizeof(buf), "%Y%m%d", &tm);
   r.emplace("Date", std::string(buf));
   std::strftime(buf, sizeof(buf), "%H%M%S", &tm);
   r.emplace("Time", std::string(buf));

   return m_logFilePattern.ReplacePlaceholders(r);
}

std::unordered_map<std::string, std::string> GlobalConfig::GetReplaceValuesDictionary(const std::string& tableFilename, const std::string& romName) const
{
   std::unordered_map<std::string, std::string> d;
   if (GetGlobalConfigFile() != nullptr)
   {
      d.emplace("GlobalConfigDirectory", GetGlobalConfigDirectory()->FullName());
      d.emplace("GlobalConfigDir", GetGlobalConfigDirectory()->FullName());
   }

   FileInfo fi(std::filesystem::current_path().string());
   d.emplace("DllDirectory", fi.Directory()->FullName());
   d.emplace("DllDir", fi.Directory()->FullName());
   d.emplace("AssemblyDirectory", fi.Directory()->FullName());
   d.emplace("AssemblyDir", fi.Directory()->FullName());

   if (!StringExtensions::IsNullOrWhiteSpace(tableFilename))
   {
      FileInfo fi(tableFilename);
      if (fi.Directory()->Exists())
      {
         d.emplace("TableDirectory", fi.Directory()->FullName());
         d.emplace("TableDir", fi.Directory()->FullName());
         d.emplace("TableDirectoryName", fi.Directory()->FullName());
         d.emplace("TableDirName", fi.Directory()->FullName());
      }
      d.emplace("TableName", StringExtensions::GetFileNameWithoutExtension(fi.FullName()));
   }

   if (!StringExtensions::IsNullOrWhiteSpace(romName))
      d.emplace("RomName", romName);

   return d;
}

std::string GlobalConfig::GetGlobalConfigDirectoryName() const
{
   DirectoryInfo* pDI = GetGlobalConfigDirectory();
   if (pDI == nullptr)
      return nullptr;
   return pDI->FullName();
}

DirectoryInfo* GlobalConfig::GetGlobalConfigDirectory() const
{
   FileInfo* pFI = GetGlobalConfigFile();
   if (pFI == nullptr)
      return nullptr;
   return pFI->Directory();
}

FileInfo* GlobalConfig::GetGlobalConfigFile() const
{
   if (StringExtensions::IsNullOrWhiteSpace(m_globalConfigFileName))
      return nullptr;
   return new FileInfo(m_globalConfigFileName);
}


GlobalConfig* GlobalConfig::GetGlobalConfigFromConfigXmlFile(const std::string& globalConfigFileName)
{
   try
   {
      if (std::filesystem::exists(globalConfigFileName))
      {
         std::string xml = FileReader::ReadFileToString(globalConfigFileName);


         GlobalConfig* pGlobalConfig = FromXml(xml);
         if (pGlobalConfig != nullptr)
            pGlobalConfig->SetGlobalConfigFilename(globalConfigFileName);
         return pGlobalConfig;
      }
      else
      {
         Log::Write(StringExtensions::Build("Global config file \"{0}\" does not exist; no global config loaded", globalConfigFileName));
         return nullptr;
      }
   }
   catch (...)
   {
      return nullptr;
   }
}

GlobalConfig* GlobalConfig::GetGlobalConfigFromGlobalConfigXml(const std::string& configXml) { return FromXml(configXml); }

void GlobalConfig::SaveGlobalConfig(const std::string& globalConfigFilename)
{
   std::string gcFileName = StringExtensions::IsNullOrWhiteSpace(globalConfigFilename) ? m_globalConfigFileName : globalConfigFilename;
   if (StringExtensions::IsNullOrWhiteSpace(gcFileName))
      throw std::runtime_error("No filename for GlobalConfig file has been supplied. Looking up the filename from the property GlobalConfigFilename failed as well");

   std::filesystem::path p(gcFileName);
   if (std::filesystem::exists(p))
   {
      auto now = std::chrono::system_clock::now();
      auto t = std::chrono::system_clock::to_time_t(now);
      std::ostringstream ts;
      ts << std::put_time(std::localtime(&t), "%Y-%m-%d %H-%M-%S");
      std::string backupName = p.stem().string() + " old (replaced " + ts.str() + ")" + p.extension().string();
      std::filesystem::copy_file(p, p.parent_path() / backupName);
   }

   DirectoryInfo* pGCDirectory = FileInfo(gcFileName).Directory();
   pGCDirectory->CreateDirectoryPath();

   StringExtensions::WriteToFile(GetGlobalConfigXml(), gcFileName, false);
}

std::string GlobalConfig::ToXml() const
{
   XMLDocument doc;
   doc.InsertEndChild(doc.NewDeclaration());

   XMLElement* element = doc.NewElement("GlobalConfig");
   doc.InsertEndChild(element);

   element = doc.NewElement("LedWizDefaultMinCommandIntervalMs");
   element->SetText(m_ledWizDefaultMinCommandIntervalMs);
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   element = doc.NewElement("LedControlMinimumEffectDurationMs");
   element->SetText(m_ledControlMinimumEffectDurationMs);
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   element = doc.NewElement("LedControlMinimumRGBEffectDurationMs");
   element->SetText(m_ledControlMinimumRGBEffectDurationMs);
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   element = doc.NewElement("PacLedDefaultMinCommandIntervalMs");
   element->SetText(m_pacLedDefaultMinCommandIntervalMs);
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   element = doc.NewElement("IniFilesPath");
   if (!m_iniFilesPath.empty())
      element->SetText(m_iniFilesPath.c_str());
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   element = doc.NewElement("ShapeDefinitionFilePattern");
   if (!m_shapeDefinitionFilePattern.GetPattern().empty())
      element->SetText(m_shapeDefinitionFilePattern.GetPattern().c_str());
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   element = doc.NewElement("CabinetConfigFilePattern");
   if (!m_cabinetConfigFilePattern.GetPattern().empty())
      element->SetText(m_cabinetConfigFilePattern.GetPattern().c_str());
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   element = doc.NewElement("TableConfigFilePatterns");
   for (auto& fp : m_tableConfigFilePatterns)
   {
      XMLElement* filePattern = doc.NewElement("FilePattern");
      filePattern->SetText(fp.GetPattern().c_str());
      element->InsertEndChild(filePattern);
   }
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   element = doc.NewElement("EnableLogging");
   element->SetText(m_enableLog);
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   element = doc.NewElement("ClearLogOnSessionStart");
   element->SetText(m_clearLogOnSessionStart);
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   element = doc.NewElement("LogFilePattern");
   element->SetText(m_logFilePattern.GetPattern().c_str());
   doc.FirstChildElement("GlobalConfig")->InsertEndChild(element);

   XMLPrinter printer;
   doc.Print(&printer);
   return std::string(printer.CStr());
}

GlobalConfig* GlobalConfig::FromXml(const std::string& configXml)
{
   XMLDocument doc;
   if (doc.Parse(configXml.c_str()) != XML_SUCCESS)
   {
      Log::Warning(StringExtensions::Build("GlobalConfig XML parse error: {0}", doc.ErrorStr()));
      return nullptr;
   }

   XMLElement* root = doc.FirstChildElement("GlobalConfig");
   if (!root)
   {
      Log::Warning("GlobalConfig root element not found");
      return nullptr;
   }

   GlobalConfig* pGlobalConfig = new GlobalConfig();

   XMLElement* element = root->FirstChildElement("LedWizDefaultMinCommandIntervalMs");
   if (element && element->GetText())
   {
      int value;
      if (element->QueryIntText(&value) == XML_SUCCESS)
         pGlobalConfig->SetLedWizDefaultMinCommandIntervalMs(value);
   }

   element = root->FirstChildElement("LedControlMinimumEffectDurationMs");
   if (element && element->GetText())
   {
      int value;
      if (element->QueryIntText(&value) == XML_SUCCESS)
         pGlobalConfig->SetLedControlMinimumEffectDurationMs(value);
   }

   element = root->FirstChildElement("LedControlMinimumRGBEffectDurationMs");
   if (element && element->GetText())
   {
      int value;
      if (element->QueryIntText(&value) == XML_SUCCESS)
         pGlobalConfig->SetLedControlMinimumRGBEffectDurationMs(value);
   }

   element = root->FirstChildElement("PacLedDefaultMinCommandIntervalMs");
   if (element && element->GetText())
   {
      int value;
      if (element->QueryIntText(&value) == XML_SUCCESS)
         pGlobalConfig->SetPacLedDefaultMinCommandIntervalMs(value);
   }

   element = root->FirstChildElement("IniFilesPath");
   if (element && element->GetText())
      pGlobalConfig->SetIniFilesPath(element->GetText());

   element = root->FirstChildElement("ShapeDefinitionFilePattern");
   if (element && element->GetText())
      pGlobalConfig->SetShapeDefinitionFilePattern(element->GetText());

   element = root->FirstChildElement("CabinetConfigFilePattern");
   if (element && element->GetText())
      pGlobalConfig->SetCabinetConfigFilePattern(element->GetText());

   element = root->FirstChildElement("TableConfigFilePatterns");
   if (element)
   {
      XMLElement* patternElement = element->FirstChildElement("FilePattern");
      while (patternElement)
      {
         if (patternElement->GetText())
            pGlobalConfig->GetTableConfigFilePatterns().push_back(FilePattern(patternElement->GetText()));

         patternElement = patternElement->NextSiblingElement("FilePattern");
      }
   }

   element = root->FirstChildElement("EnableLogging");
   if (element && element->GetText())
   {
      bool value;
      if (element->QueryBoolText(&value) == XML_SUCCESS)
         pGlobalConfig->SetEnableLogging(value);
   }

   element = root->FirstChildElement("ClearLogOnSessionStart");
   if (element && element->GetText())
   {
      bool value;
      if (element->QueryBoolText(&value) == XML_SUCCESS)
         pGlobalConfig->SetClearLogOnSessionStart(value);
   }

   element = root->FirstChildElement("LogFilePattern");
   if (element && element->GetText())
      pGlobalConfig->SetLogFilePattern(element->GetText());

   return pGlobalConfig;
}

}
