
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
  ~Table() {}

  TableElementList* GetTableElements() { return m_pTableElements; }
  void SetTableElements(TableElementList* pTableElements) { m_pTableElements = pTableElements; }
  Pinball* GetPinball() { return m_pPinball; }
  void SetPinball(Pinball* pPinball) { m_pPinball = pPinball; }
  const std::string& GetTableName() { return m_szTableName; }
  void SetTableName(const std::string& szTableName);
  const std::string& GetRomName() { return m_szRomName; }
  void SetRomName(const std::string& szRomName);
  const std::string& GetTableFilename() { return m_szTableFilename; }
  void SetTableFilename(const std::string& szTableFilename) { m_szTableFilename = szTableFilename; }
  const std::string& GetTableConfigurationFilename() { return m_szTableConfigurationFilename; }
  void SetTableConfigurationFilename(const std::string& szTableConfigurationFilename)
  {
    m_szTableConfigurationFilename = szTableConfigurationFilename;
  }
  bool IsAddLedControlConfig() { return m_addLedControlConfig; }
  void SetAddLedControlConfig(bool addLedControlConfig) { m_addLedControlConfig = addLedControlConfig; }
  TableConfigSourceEnum GetConfigurationSource() { return m_configurationSource; }
  void SetConfigurationSource(TableConfigSourceEnum configurationSource)
  {
    m_configurationSource = configurationSource;
  }
  EffectList* GetEffects() { return m_pEffects; }
  void SetEffects(EffectList* pEffects) { m_pEffects = pEffects; }
  AssignedEffectList* GetAssignedStaticEffects() { return m_pAssignedStaticEffects; }
  void SetAssignedStaticEffects(AssignedEffectList* pAssignedStaticEffects)
  {
    m_pAssignedStaticEffects = pAssignedStaticEffects;
  }
  void UpdateTableElement(TableElementData* pData);
  void TriggerStaticEffects();
  void Init(Pinball* pPinball);
  void Finish();
  std::string GetConfigXml();
  void SaveConfigXmlFile(const std::string& szFileName);
  static Table* GetTableFromConfigXmlFile(const std::string& szFileName);
  static Table* GetTableFromConfigXml(const std::string& szConfigXml);

 private:
  TableElementList* m_pTableElements;
  Pinball* m_pPinball;
  std::string m_szTableName;
  std::string m_szRomName;
  std::string m_szTableFilename;
  std::string m_szTableConfigurationFilename;
  bool m_addLedControlConfig;
  TableConfigSourceEnum m_configurationSource;
  EffectList* m_pEffects;
  AssignedEffectList* m_pAssignedStaticEffects;
};

}  // namespace DOF