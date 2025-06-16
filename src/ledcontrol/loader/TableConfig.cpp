#include "TableConfig.h"
#include "TableConfigColumnList.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include <stdexcept>

namespace DOF
{

void TableConfig::ParseLedControlDataLine(const std::string& ledControlData, bool throwExceptions)
{
   std::vector<std::string> dataColumns = StringExtensions::Split(ledControlData, { ',' });
   if (dataColumns.size() < 1)
   {
      Log::Warning(StringExtensions::Build("No data found in line {0}", ledControlData));
      if (throwExceptions)
         throw std::runtime_error(StringExtensions::Build("No usable data found in line {0}", ledControlData));
      return;
   }

   if (StringExtensions::IsNullOrWhiteSpace(dataColumns[0]))
   {
      Log::Warning(StringExtensions::Build("No short Rom name found in line {0}", ledControlData));
      if (throwExceptions)
         throw std::runtime_error(StringExtensions::Build("No short Rom name found in line {0}", ledControlData));
      return;
   }

   m_shortRomName = dataColumns[0];

   m_columns = new TableConfigColumnList();

   m_columns->ParseControlData(ledControlData, throwExceptions);
}

void TableConfig::SetColumns(TableConfigColumnList* columns)
{
   if (m_columns != nullptr)
      delete m_columns;
   m_columns = columns;
}

TableConfig::TableConfig() { m_columns = new TableConfigColumnList(); }

TableConfig::TableConfig(const std::string& ledControlData, bool throwExceptions)
   : TableConfig()
{
   ParseLedControlDataLine(ledControlData, throwExceptions);
}

TableConfig::~TableConfig() { delete m_columns; }

}