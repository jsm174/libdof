
#pragma once

#include <unordered_map>
#include "DOF/DOF.h"
#include "TableConfigSourceEnum.h"
#include "TableElementTypeEnum.h"

namespace DOF
{

class Pinball;
class TableElementData;
class TableElementList;
class AssignedEffectList;
class EffectList;
class ShapeDefinitions;

class Table
{
public:
   Table();
   ~Table();

   Table(const Table&) = delete;
   Table& operator=(const Table&) = delete;

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
   ShapeDefinitions* GetShapeDefinitions() { return m_pShapeDefinitions; }
   void SetShapeDefinitions(ShapeDefinitions* pShapeDefinitions) { m_pShapeDefinitions = pShapeDefinitions; }
   void UpdateTableElement(TableElementData* pData);
   void UpdateTableElement(const std::string& elementName, int value);
   void UpdateTableElement(TableElementTypeEnum elementType, int number, int value);
   void TriggerStaticEffects();
   void Init(Pinball* pPinball);
   void Finish();
   std::string GetConfigXml() const;
   static Table* GetTableFromConfigXmlFile(const std::string& filename);
   static Table* GetTableFromConfigXml(const std::string& configXml);
   void SaveConfigXmlFile(const std::string& filename);

   std::string ToXml() const;
   static Table* FromXml(const std::string& xml);

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
   ShapeDefinitions* m_pShapeDefinitions;
};

}