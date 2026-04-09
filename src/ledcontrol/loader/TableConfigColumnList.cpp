#include "TableConfigColumnList.h"
#include "TableConfigColumn.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include <algorithm>

namespace DOF
{

void TableConfigColumnList::ParseControlData(const std::string& ledControlData, bool throwExceptions)
{
   std::vector<std::string> cols = SplitColumns(ledControlData);
   if (cols.size() < 2)
   {
      Log::Warning(StringExtensions::Build("No data to parse found in LedControlData: {0}", ledControlData));
      if (throwExceptions)
         throw std::runtime_error(StringExtensions::Build("No data to parse found in LedControlData: {0}", ledControlData));
      return;
   }
   int firstOutputNumber = 1;
   int lastColumnWithData = -1;
   for (int i = 1; i < static_cast<int>(cols.size()); i++)
   {
      if (!StringExtensions::IsNullOrWhiteSpace(cols[i]))
         lastColumnWithData = i;
   }
   for (int i = 1; i <= lastColumnWithData; i++)
   {
      TableConfigColumn* c = new TableConfigColumn();
      c->SetNumber(i);
      c->SetFirstOutputNumber(firstOutputNumber);
      bool parseOK = c->ParseColumnData(cols[i], false);
      if (!parseOK)
      {
         Log::Warning(StringExtensions::Build("Previous exceptions occured in the line {0} of the ledcontrol file", ledControlData));
         if (throwExceptions)
         {
            delete c;
            throw std::runtime_error(StringExtensions::Build("Exception(s) occured when parsing {0}", ledControlData));
         }
      }

      push_back(c);

      firstOutputNumber += c->GetRequiredOutputCount();
   }
}

std::vector<std::string> TableConfigColumnList::SplitColumns(const std::string& configData)
{
   std::vector<std::string> l;

   int bracketCount = 0;

   int lp = 0;

   for (int p = 0; p < static_cast<int>(configData.length()); p++)
   {
      if (configData[p] == '(')
         bracketCount++;
      else if (configData[p] == ')')
         bracketCount--;
      if (configData[p] == ',' && bracketCount <= 0)
      {
         l.push_back(configData.substr(lp, p - lp));
         lp = p + 1;
         bracketCount = 0;
      }
   }

   if (lp < static_cast<int>(configData.length()))
      l.push_back(configData.substr(lp));

   return l;
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