#pragma once

#include "DOF/DOF.h"
#include "TableOverrideSettingDevice.h"
#include <string>
#include <vector>

namespace DOF
{

class TableOverrideSetting
{
public:
   TableOverrideSetting();
   TableOverrideSetting(const std::string& name, bool enabled);
   ~TableOverrideSetting();

   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name) { m_name = name; }

   bool IsEnabled() const { return m_enabled; }
   void SetEnabled(bool enabled) { m_enabled = enabled; }

   bool IsActiveSetting() const { return m_activeSetting; }
   void SetActiveSetting(bool activeSetting) { m_activeSetting = activeSetting; }

   const std::string& GetRoms() const { return m_roms; }
   void SetRoms(const std::string& roms);

   const std::vector<std::string>& GetRomList() const { return m_romList; }

   const std::string& GetTables() const { return m_tables; }
   void SetTables(const std::string& tables);

   const std::vector<std::string>& GetTableList() const { return m_tableList; }

   const std::vector<TableOverrideSettingDevice*>& GetTableOverrideSettingDeviceList() const { return m_tableOverrideSettingDeviceList; }
   void AddTableOverrideSettingDevice(TableOverrideSettingDevice* device);
   void ClearTableOverrideSettingDevices();

private:
   std::string m_name;
   bool m_enabled;
   bool m_activeSetting;
   std::string m_roms;
   std::vector<std::string> m_romList;
   std::string m_tables;
   std::vector<std::string> m_tableList;
   std::vector<TableOverrideSettingDevice*> m_tableOverrideSettingDeviceList;

   void ParseRoms();
   void ParseTables();
};

}