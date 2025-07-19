#pragma once

#include "ICabinetOwner.h"
#include <unordered_map>
#include <string>

namespace DOF
{

class AlarmHandler;

class CabinetOwner : public ICabinetOwner
{
public:
   CabinetOwner();
   ~CabinetOwner() override;


   const std::unordered_map<std::string, std::string>& GetConfigurationSettings() const override { return m_configurationSettings; }
   std::unordered_map<std::string, std::string>& GetConfigurationSettings() override { return m_configurationSettings; }

   void AddConfigurationSetting(const std::string& key, const std::string& value) override;
   void AddConfigurationSetting(const std::string& key, int value) override;
   bool HasConfigurationSetting(const std::string& key) const override;
   std::string GetConfigurationSetting(const std::string& key, const std::string& defaultValue = "") const override;
   int GetConfigurationSettingInt(const std::string& key, int defaultValue = 0) const override;
   void RemoveConfigurationSetting(const std::string& key) override;
   void ClearConfigurationSettings() override;


   void SetAlarms(AlarmHandler* alarms) override { m_alarms = alarms; }
   AlarmHandler* GetAlarms() override { return m_alarms; }

private:
   std::unordered_map<std::string, std::string> m_configurationSettings;
   AlarmHandler* m_alarms;
};

}