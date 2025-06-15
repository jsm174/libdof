#pragma once

#include <string>
#include <vector>
#include "DOF/DOF.h"

namespace DOF
{

class TableConfigSetting;

class TableConfigColumn : public std::vector<TableConfigSetting*>
{
public:
   int GetNumber() const { return m_number; }
   void SetNumber(int number) { m_number = number; }
   int GetFirstOutputNumber() const { return m_firstOutputNumber; }
   void SetFirstOutputNumber(int firstOutputNumber) { m_firstOutputNumber = firstOutputNumber; }
   int GetRequiredOutputCount() const;
   bool IsArea() const;
   bool ParseColumnData(const std::string& columnData, bool throwExceptions = false);

   TableConfigColumn(int columnNumber, const std::string& columnData, bool throwExceptions = false);
   TableConfigColumn();
   ~TableConfigColumn();

private:
   std::vector<std::string> SplitSettings(const std::string& configData);
   int m_number;
   int m_firstOutputNumber;
};

}