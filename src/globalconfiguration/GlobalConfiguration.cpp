#include "GlobalConfiguration.h"

/*

<?xml version="1.0" encoding="utf-8"?>
<!--Global configuration for the DirectOutput framework.-->
<GlobalConfig>
  <LedControlIniFiles />
  <CabinetConfigFilePatterns>
    <FilePattern>{GlobalConfigDir}\cabinet.xml</FilePattern>
  </CabinetConfigFilePatterns>
  <CabinetScriptFilePatterns></CabinetScriptFilePatterns>
  <TableScriptFilePatterns></TableScriptFilePatterns>
  <TableConfigFilePatterns>
    <FilePattern>{GlobalConfigDir}\{TableName}.xml</FilePattern>
  </TableConfigFilePatterns>
  <EnableLogging>true</EnableLogging>
  <LogFilePattern>{DllDir}\DirectOutput.log</LogFilePattern>
  <GlobalScriptFilePatterns />
</GlobalConfig>

*/

namespace DOF
{

GlobalConfiguration* GlobalConfiguration::m_pInstance = NULL;

GlobalConfiguration* GlobalConfiguration::GetInstance()
{
  if (!m_pInstance) m_pInstance = new GlobalConfiguration();

  return m_pInstance;
}

FILE* GlobalConfiguration::GetCabinetConfigFile()
{
  // if (!CabinetConfigFilePattern.Pattern.IsNullOrWhiteSpace() && CabinetConfigFilePattern.IsValid)
  //    return CabinetConfigFilePattern.GetFirstMatchingFile(GetReplaceValuesDictionary());

  return NULL;
}

}  // namespace DOF
