#pragma once

#include <map>

#include "DOF/DOF.h"

namespace DOF
{

class AlarmHandler;

class ICabinetOwner
{
public:
   ICabinetOwner() { }
   ~ICabinetOwner() { }

   const std::map<std::string, std::string>& GetConfigurationSettings() const { return m_configurationSettings; }
   void SetAlarms(AlarmHandler* pAlarms) { m_pAlarms = pAlarms; }
   AlarmHandler* GetAlarms() { return m_pAlarms; }

private:
   std::map<std::string, std::string> m_configurationSettings;
   AlarmHandler* m_pAlarms;
};

} // namespace DOF
