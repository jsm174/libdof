#include "TableOverrideSetting.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include <algorithm>
#include <sstream>

namespace DOF
{

TableOverrideSetting::TableOverrideSetting()
   : m_enabled(false)
   , m_activeSetting(false)
{
}

TableOverrideSetting::TableOverrideSetting(const std::string& name, bool enabled)
   : m_name(name)
   , m_enabled(enabled)
   , m_activeSetting(false)
{
   Log::Write(StringExtensions::Build("TableOverrideSetting constructor...name={0}", name));
}

TableOverrideSetting::~TableOverrideSetting() { ClearTableOverrideSettingDevices(); }

void TableOverrideSetting::SetRoms(const std::string& roms)
{
   m_roms = roms;
   ParseRoms();
}

void TableOverrideSetting::ParseRoms()
{
   m_romList.clear();

   if (m_roms.empty())
      return;

   std::stringstream ss(m_roms);
   std::string item;

   while (std::getline(ss, item, ','))
   {

      item.erase(0, item.find_first_not_of(" \t"));
      item.erase(item.find_last_not_of(" \t") + 1);

      if (!item.empty())
      {
         m_romList.push_back(item);
      }
   }
}

void TableOverrideSetting::SetTables(const std::string& tables)
{
   m_tables = tables;
   ParseTables();
}

void TableOverrideSetting::ParseTables()
{
   m_tableList.clear();

   if (m_tables.empty())
      return;

   std::stringstream ss(m_tables);
   std::string item;

   while (std::getline(ss, item, ','))
   {

      item.erase(0, item.find_first_not_of(" \t"));
      item.erase(item.find_last_not_of(" \t") + 1);

      if (!item.empty())
      {
         m_tableList.push_back(item);
      }
   }
}

void TableOverrideSetting::AddTableOverrideSettingDevice(TableOverrideSettingDevice* device)
{
   if (device)
   {
      m_tableOverrideSettingDeviceList.push_back(device);
   }
}

void TableOverrideSetting::ClearTableOverrideSettingDevices()
{

   for (auto* device : m_tableOverrideSettingDeviceList)
   {
      delete device;
   }
   m_tableOverrideSettingDeviceList.clear();
}

}