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

   int GetLedWizDefaultMinCommandIntervalMs() const { return m_ledWizDefaultMinCommandIntervalMs; }
   void SetLedWizDefaultMinCommandIntervalMs(int value);
   int GetLedControlMinimumEffectDurationMs() const { return m_ledControlMinimumEffectDurationMs; }
   void SetLedControlMinimumEffectDurationMs(int value) { m_ledControlMinimumEffectDurationMs = value; }
   int GetLedControlMinimumRGBEffectDurationMs() const { return m_ledControlMinimumRGBEffectDurationMs; }
   void SetLedControlMinimumRGBEffectDurationMs(int value) { m_ledControlMinimumRGBEffectDurationMs = value; }
   int GetPacLedDefaultMinCommandIntervalMs() const { return m_pacLedDefaultMinCommandIntervalMs; }
   void SetPacLedDefaultMinCommandIntervalMs(int value);
   const std::string& GetIniFilesPath() const { return m_iniFilesPath; }
   void SetIniFilesPath(const std::string& path) { m_iniFilesPath = path; }
   std::unordered_map<int, FileInfo> GetIniFilesDictionary(const std::string& tableFilename = "") const;
   FileInfo* GetTableMappingFile(const std::string& tableFilename = "") const;
   const FilePattern& GetShapeDefinitionFilePattern() const { return m_shapeDefinitionFilePattern; }
   void SetShapeDefinitionFilePattern(const std::string& pattern) { m_shapeDefinitionFilePattern.SetPattern(pattern); }
   FileInfo* GetShapeDefinitionFile(const std::string& tableFilename = "", const std::string& romName = "") const;
   const FilePattern& GetCabinetConfigFilePattern() const { return m_cabinetConfigFilePattern; }
   void SetCabinetConfigFilePattern(const std::string& pattern) { m_cabinetConfigFilePattern.SetPattern(pattern); }
   FileInfo* GetCabinetConfigFile() const;
   DirectoryInfo GetCabinetConfigDirectory() const;
   const FilePatternList& GetTableConfigFilePatterns() const { return m_tableConfigFilePatterns; }
   FilePatternList& GetTableConfigFilePatterns() { return m_tableConfigFilePatterns; }
   FileInfo* GetTableConfigFile(const std::string& fullTableFilename) const;
   bool IsEnableLogging() const { return m_enableLog; }
   void SetEnableLogging(bool enable) { m_enableLog = enable; }
   bool IsClearLogOnSessionStart() const { return m_clearLogOnSessionStart; }
   void SetClearLogOnSessionStart(bool clear) { m_clearLogOnSessionStart = clear; }
   const FilePattern& GetLogFilePattern() const { return m_logFilePattern; }
   void SetLogFilePattern(const std::string& pattern) { m_logFilePattern.SetPattern(pattern); }
   std::string GetLogFilename(const std::string& tableFilename = "", const std::string& romName = "") const;
   std::unordered_map<std::string, std::string> GetReplaceValuesDictionary(const std::string& tableFilename = "", const std::string& romName = "") const;
   std::string GlobalConfigDirectoryName() const;
   DirectoryInfo GetGlobalConfigDirectory() const;
   const std::string& GetGlobalConfigFilename() const { return m_globalConfigFileName; }
   void SetGlobalConfigFilename(const std::string& filename) { m_globalConfigFileName = filename; }
   FileInfo* GetGlobalConfigFile() const;
   std::string GetGlobalConfigXml() const { return ToXml(); }
   static GlobalConfig* GetGlobalConfigFromConfigXmlFile(const std::string& globalConfigFileName);
   static GlobalConfig* GetGlobalConfigFromGlobalConfigXml(const std::string& configXml);
   void SaveGlobalConfig(const std::string& globalConfigFileName = "");

   std::string ToXml() const;
   static GlobalConfig* FromXml(const std::string& xml);

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

}
