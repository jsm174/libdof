#include "CabinetOwner.h"
#include <string>

namespace DOF
{

CabinetOwner::CabinetOwner()
   : m_alarms(nullptr)
{
}

CabinetOwner::~CabinetOwner() { }

void CabinetOwner::AddConfigurationSetting(const std::string& key, const std::string& value) { m_configurationSettings[key] = value; }

void CabinetOwner::AddConfigurationSetting(const std::string& key, int value) { m_configurationSettings[key] = std::to_string(value); }

bool CabinetOwner::HasConfigurationSetting(const std::string& key) const { return m_configurationSettings.find(key) != m_configurationSettings.end(); }

std::string CabinetOwner::GetConfigurationSetting(const std::string& key, const std::string& defaultValue) const
{
   auto it = m_configurationSettings.find(key);
   return (it != m_configurationSettings.end()) ? it->second : defaultValue;
}

int CabinetOwner::GetConfigurationSettingInt(const std::string& key, int defaultValue) const
{
   auto it = m_configurationSettings.find(key);
   if (it != m_configurationSettings.end())
   {
      try
      {
         return std::stoi(it->second);
      }
      catch (...)
      {
         return defaultValue;
      }
   }
   return defaultValue;
}

void CabinetOwner::RemoveConfigurationSetting(const std::string& key) { m_configurationSettings.erase(key); }

void CabinetOwner::ClearConfigurationSettings() { m_configurationSettings.clear(); }

}