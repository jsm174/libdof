#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "DOF/DOF.h"
#include "LedControlConfigList.h"
#include "TableConfigList.h"
#include "ColorConfigList.h"
#include "TableVariablesDictionary.h"
#include "VariablesDictionary.h"

namespace DOF
{

class LedControlConfig
{
public:
   LedControlConfig();
   LedControlConfig(const std::string& ledControlIniFilename, int ledWizNumber, bool throwExceptions = false);
   ~LedControlConfig();

   int GetLedWizNumber() const { return m_ledWizNumber; }
   void SetLedWizNumber(int number) { m_ledWizNumber = number; }
   const std::string& GetMinDOFVersion() const { return m_minDOFVersion; }
   void SetMinDOFVersion(const std::string& version) { m_minDOFVersion = version; }
   bool HasMinDOFVersion() const { return !m_minDOFVersion.empty(); }
   TableConfigList* GetTableConfigurations() const { return m_tableConfigurations; }
   void SetTableConfigurations(TableConfigList* configs) { m_tableConfigurations = configs; }
   ColorConfigList* GetColorConfigurations() const { return m_colorConfigurations; }
   void SetColorConfigurations(ColorConfigList* configs) { m_colorConfigurations = configs; }
   const std::string& GetLedControlIniFile() const { return m_ledControlIniFile; }

private:
   void ParseLedControlIni(const std::string& ledControlIniFilename, bool throwExceptions = false);
   std::vector<std::string> GetSection(const std::unordered_map<std::string, std::vector<std::string>>& sections, const std::vector<std::string>& sectionStartStrings);
   void ResolveTableVariables(std::vector<std::string>& dataToResolve, const std::vector<std::string>& variableData);
   void ResolveVariables(std::vector<std::string>& dataToResolve, const std::vector<std::string>& variableData);
   void ResolveRGBColors();

   int m_ledWizNumber;
   std::string m_minDOFVersion;
   TableConfigList* m_tableConfigurations;
   ColorConfigList* m_colorConfigurations;
   std::string m_ledControlIniFile;
};

}
