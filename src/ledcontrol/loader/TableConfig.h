#pragma once

#include <string>
#include "DOF/DOF.h"

namespace DOF
{

class TableConfigColumnList;

class TableConfig
{
public:
   const std::string& GetShortRomName() const { return m_shortRomName; }
   void SetShortRomName(const std::string& shortRomName) { m_shortRomName = shortRomName; }
   TableConfigColumnList* GetColumns() const { return m_columns; }
   void SetColumns(TableConfigColumnList* columns);
   void ParseLedControlDataLine(const std::string& ledControlData, bool throwExceptions);

   TableConfig();
   TableConfig(const std::string& ledControlData, bool throwExceptions);

   ~TableConfig();

private:
   std::string m_shortRomName;
   TableConfigColumnList* m_columns;
};

}