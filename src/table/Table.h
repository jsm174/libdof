
#pragma once

#include "DOF/DOF.h"
#include "TableConfigSourceEnum.h"

namespace DOF
{

class Pinball;
class TableElementData;
class TableElementList;
class AssignedEffectList;
class EffectList;

class Table
{
public:
   Table();
   ~Table() { }

   TableElementList* GetTableElements() { return m_pTableElements; }
   void SetTableElements(TableElementList* pTableElements) { m_pTableElements = pTableElements; }
   Pinball* GetPinball() { return m_pPinball; }
   void SetPinball(Pinball* pPinball) { m_pPinball = pPinball; }
   const std::string& GetTableName() { return m_tableName; }
   void SetTableName(const std::string& tableName);
   const std::string& GetRomName() { return m_romName; }
   void SetRomName(const std::string& romName);
   const std::string& GetTableFilename() { return m_tableFilename; }
   void SetTableFilename(const std::string& tableFilename) { m_tableFilename = tableFilename; }
   const std::string& GetTableConfigurationFilename() { return m_tableConfigurationFilename; }
   void SetTableConfigurationFilename(const std::string& tableConfigurationFilename) { m_tableConfigurationFilename = tableConfigurationFilename; }
   bool IsAddLedControlConfig() { return m_addLedControlConfig; }
   void SetAddLedControlConfig(bool addLedControlConfig) { m_addLedControlConfig = addLedControlConfig; }
   TableConfigSourceEnum GetConfigurationSource() { return m_configurationSource; }
   void SetConfigurationSource(TableConfigSourceEnum configurationSource) { m_configurationSource = configurationSource; }
   EffectList* GetEffects() { return m_pEffects; }
   void SetEffects(EffectList* pEffects) { m_pEffects = pEffects; }
   AssignedEffectList* GetAssignedStaticEffects() { return m_pAssignedStaticEffects; }
   void SetAssignedStaticEffects(AssignedEffectList* pAssignedStaticEffects) { m_pAssignedStaticEffects = pAssignedStaticEffects; }
   void UpdateTableElement(TableElementData* pData);
   void TriggerStaticEffects();
   void Init(Pinball* pPinball);
   void Finish();
   std::string GetConfigXml();
   void SaveConfigXmlFile(const std::string& filename);
   static Table* GetTableFromConfigXmlFile(const std::string& filename);
   static Table* GetTableFromConfigXml(const std::string& configXml);

private:
   TableElementList* m_pTableElements;
   Pinball* m_pPinball;
   std::string m_tableName;
   std::string m_romName;
   std::string m_tableFilename;
   std::string m_tableConfigurationFilename;
   bool m_addLedControlConfig;
   TableConfigSourceEnum m_configurationSource;
   EffectList* m_pEffects;
   AssignedEffectList* m_pAssignedStaticEffects;
};

} // namespace DOF