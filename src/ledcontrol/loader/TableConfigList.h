#pragma once

#include <vector>
#include <string>

#include "DOF/DOF.h"
#include "TableConfig.h"

namespace DOF
{

class TableConfigList
{
public:
   TableConfigList();
   ~TableConfigList();
   void Add(TableConfig* config);
   size_t Size() const { return m_configs.size(); }
   TableConfig* operator[](size_t index) { return m_configs[index]; }
   const TableConfig* operator[](size_t index) const { return m_configs[index]; }
   bool Contains(const std::string& romName) const;
   void ParseLedcontrolData(const std::vector<std::string>& ledControlData, bool throwExceptions = false);
   void ParseLedcontrolData(const std::string& ledControlData, bool throwExceptions = false);
   std::vector<TableConfig*>::iterator begin() { return m_configs.begin(); }
   std::vector<TableConfig*>::iterator end() { return m_configs.end(); }
   std::vector<TableConfig*>::const_iterator begin() const { return m_configs.begin(); }
   std::vector<TableConfig*>::const_iterator end() const { return m_configs.end(); }

private:
   std::vector<TableConfig*> m_configs;
};

}