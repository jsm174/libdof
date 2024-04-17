#include "Table.h"

#include "../Log.h"
#include "../fx/AssignedEffectList.h"
#include "../fx/EffectList.h"
#include "TableElementList.h"

namespace DOF
{
Table::Table()
{
  m_pPinball = nullptr;
  m_addLedControlConfig = false;
  m_configurationSource = TableConfigSourceEnum::Unknown;
  m_pEffects = new EffectList();
  m_pTableElements = new TableElementList();
  m_pAssignedStaticEffects = new AssignedEffectList();
}

void Table::SetTableName(const std::string& szTableName)
{
  if (szTableName != m_szTableName)
  {
    m_szTableName = szTableName;

    // if (TableNameChanged)
    //{
    //    TableNameChanged(this, new EventArgs());
    // }
  }
}

void Table::SetRomName(const std::string& szRomName)
{
  if (szRomName != m_szRomName)
  {
    m_szRomName = szRomName;

    // if (RomNameChanged)
    //{
    //    RomNameChanged(this, new EventArgs());
    // }
  }
}

void Table::UpdateTableElement(TableElementData* pData) { m_pTableElements->UpdateState(pData); }

void Table::TriggerStaticEffects()
{
  // m_pAssignedStaticEffects->Trigger(new TableElementData(TableElementTypeEnum::Unknown, 0, 1));
}

void Table::Init(Pinball* pPinball)
{
  m_pPinball = pPinball;

  // Loading shape definition file

  // m_pEffects->Init(this);

  // m_pTableElements->InitAssignedEffects(this);
  // m_pAssignedStaticEffects->Init(this);
}

std::string Table::GetConfigXml() { return ""; }

void Table::SaveConfigXmlFile(const std::string& szFileName) {}

Table* Table::GetTableFromConfigXmlFile(const std::string& szFileName)
{
  Log::Write("Not implemented");

  return nullptr;
}

Table* Table::GetTableFromConfigXml(const std::string& szConfigXml)
{
  Log::Write("Not implemented");

  return nullptr;
}

void Table::Finish()
{
  m_pAssignedStaticEffects->Finish();
  m_pTableElements->FinishAssignedEffects();
  m_pEffects->Finish();
  m_pPinball = nullptr;
}

}  // namespace DOF