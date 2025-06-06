#pragma once

#include "DOF/DOF.h"

#include "../general/FileInfo.h"
#include "../general/FilePattern.h"
#include "../general/FilePatternList.h"

#include <algorithm>
#include <unordered_map>

namespace DOF
{

class GlobalConfig
{
public:
   GlobalConfig();
   ~GlobalConfig() { };

   int GetLedWizDefaultMinCommandIntervalMs() { return m_ledWizDefaultMinCommandIntervalMs; }
   void SetLedWizDefaultMinCommandIntervalMs(int value);
   int GetLedControlMinimumEffectDurationMs() { return m_ledControlMinimumEffectDurationMs; }
   void SetLedControlMinimumEffectDurationMs(int value) { m_ledControlMinimumEffectDurationMs = value; }
   int GetLedControlMinimumRGBEffectDurationMs() { return m_ledControlMinimumRGBEffectDurationMs; }
   void SetLedControlMinimumRGBEffectDurationMs(int value) { m_ledControlMinimumRGBEffectDurationMs = value; }
   int GetPacLedDefaultMinCommandIntervalMs() { return m_pacLedDefaultMinCommandIntervalMs; }
   void SetPacLedDefaultMinCommandIntervalMs(int value);
   const std::string& GetIniFilesPath() { return m_iniFilesPath; }
   void SetIniFilesPath(std::string path) { m_iniFilesPath = path; }
   std::unordered_map<int, FileInfo> GetIniFilesDictionary(const std::string& tableFilename);
   FileInfo* GetTableMappingFile(const std::string& tableFilename);
   const FilePattern& GetShapeDefinitionFilePattern() { return m_shapeDefinitionFilePattern; }
   void SetShapeDefinitionFilePattern(const std::string& pattern) { m_shapeDefinitionFilePattern.SetPattern(pattern); }
   FileInfo* GetShapeDefinitionFile(const std::string& tableFilename = "", const std::string& romName = "");
   const FilePattern& GetCabinetConfigFilePattern() { return m_cabinetConfigFilePattern; }
   void SetCabinetConfigFilePattern(const std::string& pattern) { m_cabinetConfigFilePattern.SetPattern(pattern); }
   FileInfo* GetCabinetConfigFile();
   DirectoryInfo* GetCabinetConfigDirectory();
   FilePatternList& GetTableConfigFilePatterns() { return m_tableConfigFilePatterns; }
   FileInfo* GetTableConfigFile(const std::string& tableFilename);
   bool IsEnableLogging() { return m_enableLog; }
   void SetEnableLogging(bool enable) { m_enableLog = enable; }
   bool IsClearLogOnSessionStart() { return m_clearLogOnSessionStart; }
   void SetClearLogOnSessionStart(bool clear) { m_clearLogOnSessionStart = clear; }
   const FilePattern& GetLogFilePattern() { return m_logFilePattern; }
   void SetLogFilePattern(const std::string& pattern) { m_logFilePattern.SetPattern(pattern); }
   std::string GetLogFilename(const std::string& tableFilename = "", const std::string& romName = "");
   std::unordered_map<std::string, std::string> GetReplaceValuesDictionary(const std::string& tableFilename = "", const std::string& romName = "");
   std::string GetGlobalConfigDirectoryName();
   DirectoryInfo* GetGlobalConfigDirectory();
   const std::string& GetGlobalConfigFilename() { return m_globalConfigFileName; }
   void SetGlobalConfigFilename(const std::string& filename) { m_globalConfigFileName = filename; }
   FileInfo* GetGlobalConfigFile();
   std::string GetGlobalConfigXml();
   static GlobalConfig* GetGlobalConfigFromConfigXmlFile(const std::string& globalConfigFileName);
   static GlobalConfig* GetGlobalConfigFromGlobalConfigXml(const std::string& configXml);
   void SaveGlobalConfig(const std::string& globalConfigFilename = "");

private:
   int m_ledWizDefaultMinCommandIntervalMs;
   int m_ledControlMinimumEffectDurationMs;
   int m_ledControlMinimumRGBEffectDurationMs;
   int m_pacLedDefaultMinCommandIntervalMs;
   std::string m_iniFilesPath;
   FilePattern m_shapeDefinitionFilePattern;
   FilePattern m_cabinetConfigFilePattern;
   FilePatternList m_tableConfigFilePatterns;
   bool m_enableLog;
   bool m_clearLogOnSessionStart;
   FilePattern m_logFilePattern;
   std::string m_globalConfigFileName;
};

} // namespace DOF
