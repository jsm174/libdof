#pragma once

#include "DOF/DOF.h"
#include "TableOverrideSetting.h"
#include "../out/IOutput.h"
#include <string>
#include <vector>

namespace DOF
{

class Output;

class TableOverrideSettings
{
public:
   static TableOverrideSettings* GetInstance();

   const std::string& GetActiveTableName() const { return m_activeTableName; }
   void SetActiveTableName(const std::string& activeTableName) { m_activeTableName = activeTableName; }

   const std::string& GetActiveRomName() const { return m_activeRomName; }
   void SetActiveRomName(const std::string& activeRomName) { m_activeRomName = activeRomName; }

   void ActivateOverrides();

   TableOverrideSettingDevice* GetActiveDevice(IOutput* currentOutput, bool recalculateOutputValue, int startingDeviceIndex, int currentDeviceIndex);
   IOutput* GetNewRecalculatedOutput(IOutput* currentOutput, int startingDeviceIndex, int currentDeviceIndex);

   const std::vector<TableOverrideSetting*>& GetSettings() const { return m_settings; }
   void AddSetting(TableOverrideSetting* setting);
   void ClearSettings();
   TableOverrideSetting* FindByName(const std::string& name);

private:
   TableOverrideSettings();
   ~TableOverrideSettings();

   static TableOverrideSettings* m_pInstance;

   std::string m_activeTableName;
   std::string m_activeRomName;
   std::vector<TableOverrideSetting*> m_settings;

   bool ContainsString(const std::vector<std::string>& list, const std::string& value) const;
   bool MatchesWildcard(const std::vector<std::string>& list, const std::string& value) const;
};

}