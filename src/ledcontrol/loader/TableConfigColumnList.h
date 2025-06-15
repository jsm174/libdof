#pragma once

#include <vector>
#include <string>
#include "DOF/DOF.h"

namespace DOF
{

class TableConfigColumn;

class TableConfigColumnList : public std::vector<TableConfigColumn*>
{
public:
   void ParseControlData(const std::string& ledControlData, bool throwExceptions = false);
   void Sort();
   TableConfigColumnList();
   TableConfigColumnList(const std::string& ledControlData, bool throwExceptions = false);

   ~TableConfigColumnList();

private:
   std::vector<std::string> SplitColumns(const std::string& configData);
};

}