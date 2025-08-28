
#pragma once

#include <unordered_map>
#include "DOF/DOF.h"
#include "TableConfigSourceEnum.h"
#include "TableElementTypeEnum.h"
#include "../general/bitmap/FastImage.h"
#include "../general/bitmap/FastImageList.h"

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

   TableElementList* GetTableElements() { return m_tableElements; }
   void SetTableElements(TableElementList* tableElements) { m_tableElements = tableElements; }
   Pinball* GetPinball() { return m_pinball; }
   void SetPinball(Pinball* pinball) { m_pinball = pinball; }
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
   EffectList* GetEffects() { return m_effects; }
   void SetEffects(EffectList* effects) { m_effects = effects; }
   AssignedEffectList* GetAssignedStaticEffects() { return m_assignedStaticEffects; }
   void SetAssignedStaticEffects(AssignedEffectList* assignedStaticEffects) { m_assignedStaticEffects = assignedStaticEffects; }
   ShapeDefinitions* GetShapeDefinitions() { return m_shapeDefinitions; }
   void SetShapeDefinitions(ShapeDefinitions* shapeDefinitions) { m_shapeDefinitions = shapeDefinitions; }
   FastImageList& GetBitmaps() { return m_bitmaps; }
   void UpdateTableElement(TableElementData* data);
   void UpdateTableElement(const std::string& elementName, int value);
   void UpdateTableElement(TableElementTypeEnum elementType, int number, int value);
   void TriggerStaticEffects();
   void Init(Pinball* pinball);
   void Finish();
   std::string GetConfigXml() const;
   static Table* GetTableFromConfigXmlFile(const std::string& filename);
   static Table* GetTableFromConfigXml(const std::string& configXml);
   void SaveConfigXmlFile(const std::string& filename);

   std::string ToXml() const;
   static Table* FromXml(const std::string& xml);

private:
   TableElementList* m_tableElements;
   Pinball* m_pinball;
   std::string m_tableName;
   std::string m_romName;
   std::string m_tableFilename;
   std::string m_tableConfigurationFilename;
   bool m_addLedControlConfig;
   TableConfigSourceEnum m_configurationSource;
   EffectList* m_effects;
   AssignedEffectList* m_assignedStaticEffects;
   ShapeDefinitions* m_shapeDefinitions;
   FastImageList m_bitmaps;
};

}