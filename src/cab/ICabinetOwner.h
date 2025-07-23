#pragma once

#include "DOF/DOF.h"

#include <unordered_map>
#include <string>

namespace DOF
{

class AlarmHandler;

class ICabinetOwner
{
public:
   ICabinetOwner() = default;
   virtual ~ICabinetOwner() = default;


   virtual const std::unordered_map<std::string, std::string>& GetConfigurationSettings() const = 0;
   virtual std::unordered_map<std::string, std::string>& GetConfigurationSettings() = 0;

   virtual void AddConfigurationSetting(const std::string& key, const std::string& value) = 0;
   virtual void AddConfigurationSetting(const std::string& key, int value) = 0;
   virtual bool HasConfigurationSetting(const std::string& key) const = 0;
   virtual std::string GetConfigurationSetting(const std::string& key, const std::string& defaultValue = "") const = 0;
   virtual int GetConfigurationSettingInt(const std::string& key, int defaultValue = 0) const = 0;
   virtual void RemoveConfigurationSetting(const std::string& key) = 0;
   virtual void ClearConfigurationSettings() = 0;

   virtual void SetAlarms(AlarmHandler* alarms) = 0;
   virtual AlarmHandler* GetAlarms() = 0;
};

}
