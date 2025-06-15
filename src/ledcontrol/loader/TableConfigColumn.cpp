#include "TableConfigColumn.h"
#include "TableConfigSetting.h"
#include "OutputControlEnum.h"
#include "OutputTypeEnum.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include <algorithm>

namespace DOF
{

int TableConfigColumn::GetRequiredOutputCount() const
{
   for (const TableConfigSetting* s : *this)
      if (s->GetOutputType() == OutputTypeEnum::RGBOutput)
         return 3;
   return 1;
}

bool TableConfigColumn::IsArea() const
{
   for (const TableConfigSetting* s : *this)
      if (s->IsArea())
         return true;
   return false;
}

bool TableConfigColumn::ParseColumnData(const std::string& columnData, bool throwExceptions)
{
   bool exceptionOccured = false;
   std::vector<std::string> columnConfigs = SplitSettings(columnData);

   for (const std::string& cc : columnConfigs)
   {
      if (!StringExtensions::IsNullOrWhiteSpace(cc))
      {
         try
         {
            TableConfigSetting* tcs = new TableConfigSetting(cc);
            if (tcs->GetOutputControl() != OutputControlEnum::FixedOff)
               push_back(tcs);
         }
         catch (const std::exception& e)
         {
            exceptionOccured = true;
            Log::Exception(StringExtensions::Build("Could not parse setting {0} in column data {1}.", cc, columnData));
            if (throwExceptions)
               throw std::runtime_error(StringExtensions::Build("Could not parse setting {0} in column data {1}.", cc, columnData));
         }
      }
   }
   return !exceptionOccured;
}

std::vector<std::string> TableConfigColumn::SplitSettings(const std::string& configData)
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
      if (configData[p] == '/' && bracketCount <= 0)
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

TableConfigColumn::TableConfigColumn(int columnNumber, const std::string& columnData, bool throwExceptions)
{
   m_number = columnNumber;
   ParseColumnData(columnData, throwExceptions);
}

TableConfigColumn::TableConfigColumn() { }

TableConfigColumn::~TableConfigColumn()
{
   for (TableConfigSetting* setting : *this)
      delete setting;
   clear();
}

}