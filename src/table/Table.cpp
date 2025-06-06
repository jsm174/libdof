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

void Table::SetTableName(const std::string& tableName)
{
   if (tableName != m_tableName)
   {
      m_tableName = tableName;

      // if (TableNameChanged)
      //{
      //    TableNameChanged(this, new EventArgs());
      // }
   }
}

void Table::SetRomName(const std::string& romName)
{
   if (romName != m_romName)
   {
      m_romName = romName;

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

void Table::SaveConfigXmlFile(const std::string& filename) { }

Table* Table::GetTableFromConfigXmlFile(const std::string& filename)
{
   Log::Write("Not implemented");

   return nullptr;
}

Table* Table::GetTableFromConfigXml(const std::string& configXml)
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

} // namespace DOF