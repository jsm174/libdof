#include "TableOverrideSettings.h"
#include "../out/Output.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include <algorithm>
#include <cctype>

namespace DOF
{

TableOverrideSettings* TableOverrideSettings::m_pInstance = nullptr;

TableOverrideSettings* TableOverrideSettings::GetInstance()
{
   if (!m_pInstance)
      m_pInstance = new TableOverrideSettings();
   return m_pInstance;
}

TableOverrideSettings::TableOverrideSettings() { }

TableOverrideSettings::~TableOverrideSettings() { ClearSettings(); }

void TableOverrideSettings::ActivateOverrides()
{
   for (auto* currentTableOverrideSetting : m_settings)
   {
      bool romMatch = ContainsString(currentTableOverrideSetting->GetRomList(), m_activeRomName);
      bool tableMatch = ContainsString(currentTableOverrideSetting->GetTableList(), m_activeTableName);

      if (romMatch || tableMatch)
         currentTableOverrideSetting->SetActiveSetting(true);
      else
         currentTableOverrideSetting->SetActiveSetting(false);

      if (!romMatch && MatchesWildcard(currentTableOverrideSetting->GetRomList(), m_activeRomName))
         currentTableOverrideSetting->SetActiveSetting(true);

      if (!tableMatch && MatchesWildcard(currentTableOverrideSetting->GetTableList(), m_activeTableName))
         currentTableOverrideSetting->SetActiveSetting(true);

      Log::Write(StringExtensions::Build(
         "TableOverrideSettings.activateOverrides... activeSetting[{0}]={1}", currentTableOverrideSetting->GetName(), currentTableOverrideSetting->IsActiveSetting() ? "true" : "false"));
   }
}

TableOverrideSettingDevice* TableOverrideSettings::GetActiveDevice(IOutput* currentOutput, bool recalculateOutputValue, int startingDeviceIndex, int currentDeviceIndex)
{
   TableOverrideSettingDevice* foundActiveDevice = nullptr;

   for (auto* tableOverrideSetting : m_settings)
   {
      if (tableOverrideSetting->IsEnabled() && tableOverrideSetting->IsActiveSetting())
      {
         for (auto* tableOverrideSettingDevice : tableOverrideSetting->GetTableOverrideSettingDeviceList())
         {
            if ((currentDeviceIndex + startingDeviceIndex) == tableOverrideSettingDevice->GetConfigPostfixID())
            {
               const auto& outputList = tableOverrideSettingDevice->GetOutputList();
               auto it = std::find(outputList.begin(), outputList.end(), currentOutput->GetNumber());
               if (it != outputList.end())
               {
                  foundActiveDevice = tableOverrideSettingDevice;
                  break;
               }
            }
         }
      }
   }

   if (foundActiveDevice && recalculateOutputValue)
   {
      double strengthFactor = foundActiveDevice->GetOutputPercent() / 100.0;
      uint8_t newValue = static_cast<uint8_t>(currentOutput->GetOutput() * strengthFactor);

      if (foundActiveDevice->GetOutputPercent() == 0 && currentOutput->GetOutput() != 0)
         currentOutput->SetOutput(0);
      else if (foundActiveDevice->GetOutputPercent() != 100)
         currentOutput->SetOutput(newValue);
   }

   return foundActiveDevice;
}

IOutput* TableOverrideSettings::GetNewRecalculatedOutput(IOutput* currentOutput, int startingDeviceIndex, int currentDeviceIndex)
{
   if (currentOutput->GetOutput() != 0)
   {
      Output* newOutput = new Output();
      newOutput->SetOutput(currentOutput->GetOutput());
      newOutput->SetNumber(currentOutput->GetNumber());

      TableOverrideSettingDevice* activeDevice = GetActiveDevice(newOutput, true, startingDeviceIndex, currentDeviceIndex);

      if (activeDevice)
         return newOutput;
      else
      {
         delete newOutput;
         return currentOutput;
      }
   }
   else
      return currentOutput;
}

void TableOverrideSettings::AddSetting(TableOverrideSetting* setting)
{
   if (setting)
      m_settings.push_back(setting);
}

void TableOverrideSettings::ClearSettings()
{
   for (auto* setting : m_settings)
      delete setting;
   m_settings.clear();
}

TableOverrideSetting* TableOverrideSettings::FindByName(const std::string& name)
{
   auto it = std::find_if(m_settings.begin(), m_settings.end(), [&name](const TableOverrideSetting* setting) { return setting->GetName() == name; });

   return (it != m_settings.end()) ? *it : nullptr;
}

bool TableOverrideSettings::ContainsString(const std::vector<std::string>& list, const std::string& value) const { return std::find(list.begin(), list.end(), value) != list.end(); }

bool TableOverrideSettings::MatchesWildcard(const std::vector<std::string>& list, const std::string& value) const
{
   std::string lowerValue = StringExtensions::ToLower(value);

   for (const auto& item : list)
   {
      std::string lowerItem = StringExtensions::ToLower(item);

      size_t wildcardPos = lowerItem.find('*');
      if (wildcardPos != std::string::npos)
      {
         std::string prefix = lowerItem.substr(0, wildcardPos);
         if (lowerValue.find(prefix) == 0)
            return true;
      }
   }

   return false;
}

}