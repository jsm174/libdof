#include "TableConfigColumnList.h"
#include "TableConfigColumn.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include <algorithm>

namespace DOF
{

void TableConfigColumnList::ParseControlData(const std::string& ledControlData, bool throwExceptions)
{
   std::vector<std::string> Cols = SplitColumns(ledControlData);
   if (Cols.size() < 2)
   {
      Log::Warning(StringExtensions::Build("No data to parse found in LedControlData: {0}", ledControlData));
      if (throwExceptions)
         throw std::runtime_error(StringExtensions::Build("No data to parse found in LedControlData: {0}", ledControlData));
      return;
   }
   int FirstOutputNumber = 1;
   int LastColumnWithData = -1;
   for (int i = 1; i < static_cast<int>(Cols.size()); i++)
   {
      if (!StringExtensions::IsNullOrWhiteSpace(Cols[i]))
         LastColumnWithData = i;
   }
   for (int i = 1; i <= LastColumnWithData; i++)
   {
      TableConfigColumn* C = new TableConfigColumn();
      C->SetNumber(i);
      C->SetFirstOutputNumber(FirstOutputNumber);
      bool ParseOK = C->ParseColumnData(Cols[i], false);
      if (!ParseOK)
      {
         Log::Warning(StringExtensions::Build("Previous exceptions occured in the line {0} of the ledcontrol file", ledControlData));
         if (throwExceptions)
         {
            delete C;
            throw std::runtime_error(StringExtensions::Build("Exception(s) occured when parsing {0}", ledControlData));
         }
      }

      push_back(C);

      FirstOutputNumber += C->GetRequiredOutputCount();
   }
}

std::vector<std::string> TableConfigColumnList::SplitColumns(const std::string& configData)
{
   std::vector<std::string> L;

   int BracketCount = 0;

   int LP = 0;

   for (int P = 0; P < static_cast<int>(configData.length()); P++)
   {
      if (configData[P] == '(')
         BracketCount++;
      else if (configData[P] == ')')
         BracketCount--;
      if (configData[P] == ',' && BracketCount <= 0)
      {
         L.push_back(configData.substr(LP, P - LP));
         LP = P + 1;
         BracketCount = 0;
      }
   }

   if (LP < static_cast<int>(configData.length()))
      L.push_back(configData.substr(LP));

   return L;
}

void TableConfigColumnList::Sort()
{
   std::sort(begin(), end(), [](const TableConfigColumn* a, const TableConfigColumn* b) { return a->GetNumber() < b->GetNumber(); });
}

TableConfigColumnList::TableConfigColumnList() { }

TableConfigColumnList::TableConfigColumnList(const std::string& ledControlData, bool throwExceptions) { ParseControlData(ledControlData, throwExceptions); }

TableConfigColumnList::~TableConfigColumnList()
{
   for (TableConfigColumn* column : *this)
      delete column;
   clear();
}

}