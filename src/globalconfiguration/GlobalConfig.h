#pragma once

#include <algorithm>
#include <map>

#include "DOF/DOF.h"

namespace DOF
{

class GlobalConfig
{
 public:
  GlobalConfig();
  ~GlobalConfig(){};

  int GetLedWizDefaultMinCommandIntervalMs() { return m_ledWizDefaultMinCommandIntervalMs; }
  void SetLedWizDefaultMinCommandIntervalMs(int value);
  int GetLedControlMinimumEffectDurationMs() { return m_ledControlMinimumEffectDurationMs; }
  void SetLedControlMinimumEffectDurationMs(int value) { m_ledControlMinimumEffectDurationMs = value; }
  int GetLedControlMinimumRGBEffectDurationMs() { return m_ledControlMinimumRGBEffectDurationMs; }
  void SetLedControlMinimumRGBEffectDurationMs(int value) { m_ledControlMinimumRGBEffectDurationMs = value; }
  int GetPacLedDefaultMinCommandIntervalMs() { return m_pacLedDefaultMinCommandIntervalMs; }
  void SetPacLedDefaultMinCommandIntervalMs(int value);
  const std::string& GetIniFilesPath() { return m_szIniFilesPath; }
  void SetIniFilesPath(std::string szPath) { m_szIniFilesPath = szPath; }
  std::map<int, std::string> GetIniFilesDictionary(const std::string& szTableFilename);
  std::string GetTableMappingFile(const std::string& szTableFilename);
  const std::string& GetShapeDefinitionFilePattern() { return m_szShapeDefinitionFilePattern; }
  void SetShapeDefinitionFilePattern(const std::string& szPattern) { m_szShapeDefinitionFilePattern = szPattern; }
  std::string GetShapeDefinitionFile(const std::string& szTableFilename, const std::string& szRomName);
  const std::string& GetCabinetConfigFilePattern() { return m_szCabinetConfigFilePattern; }
  void SetCabinetConfigFilePattern(const std::string& szPattern) { m_szCabinetConfigFilePattern = szPattern; }
  std::string GetCabinetConfigFile();
  std::string GetCabinetConfigDirectory();
  const std::string& GetTableConfigFilePattern() { return m_szTableConfigFilePattern; }
  void SetTableConfigFilePattern(const std::string& szPattern) { m_szTableConfigFilePattern = szPattern; }
  std::string GetTableConfigFile(const std::string& szTableFilename);
  bool IsEnableLogging() { return m_enableLog; }
  void SetEnableLogging(bool enable) { m_enableLog = enable; }
  bool IsClearLogOnSessionStart() { return m_clearLogOnSessionStart; }
  void SetClearLogOnSessionStart(bool clear) { m_clearLogOnSessionStart = clear; }
  const std::string& GetLogFilePattern() { return m_szLogFilePattern; }
  void SetLogFilePattern(const std::string& szPattern) { m_szLogFilePattern = szPattern; }
  std::string GetLogFilename(const std::string& szTableFilename, const std::string& szRomName);
  std::map<std::string, std::string> GetReplaceValuesDictionary(const std::string& szTableFilename,
                                                                const std::string& szRomName);
  std::string GetGlobalConfigDirectoryName();
  std::string GetGlobalConfigDirectory();
  const std::string& GetGlobalConfigFilename() { return m_szGlobalConfigFilename; }
  void SetGlobalConfigFilename(const std::string& szFilename) { m_szGlobalConfigFilename = szFilename; }
  std::string GetGlobalConfigFile();
  std::string GetGlobalConfigXml();
  static GlobalConfig* GetGlobalConfigFromConfigXmlFile(const std::string& szGlobalConfigFileName);
  static GlobalConfig* GetGlobalConfigFromGlobalConfigXml(const std::string& szConfigXml);
  void SaveGlobalConfig(const std::string& szFilename);

 private:
  int m_ledWizDefaultMinCommandIntervalMs;
  int m_ledControlMinimumEffectDurationMs;
  int m_ledControlMinimumRGBEffectDurationMs;
  int m_pacLedDefaultMinCommandIntervalMs;
  std::string m_szIniFilesPath;
  std::string m_szShapeDefinitionFilePattern;
  std::string m_szCabinetConfigFilePattern;
  std::string m_szTableConfigFilePattern;
  bool m_enableLog;
  bool m_clearLogOnSessionStart;
  std::string m_szLogFilePattern;
  std::string m_szGlobalConfigFilename;
};

}  // namespace DOF
