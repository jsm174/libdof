#include "TableConfigList.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"

namespace DOF
{

TableConfigList::TableConfigList() { }

TableConfigList::~TableConfigList()
{
   for (TableConfig* config : m_configs)
      delete config;
   m_configs.clear();
}

void TableConfigList::ParseLedcontrolData(const std::vector<std::string>& ledControlData, bool throwExceptions)
{
   for (const std::string& data : ledControlData)
   {
      if (!StringExtensions::IsNullOrWhiteSpace(data))
         ParseLedcontrolData(data, throwExceptions);
   }
}

void TableConfigList::ParseLedcontrolData(const std::string& ledControlData, bool throwExceptions)
{
   TableConfig* tc = nullptr;

   try
   {
      tc = new TableConfig(ledControlData, throwExceptions);
   }
   catch (const std::exception& e)
   {
      Log::Exception(StringExtensions::Build("Could not load table config from data line: {0}", ledControlData));
      if (throwExceptions)
         throw std::runtime_error(StringExtensions::Build("Could not load table config from data line: {0}", ledControlData));
      return;
   }

   if (tc != nullptr)
   {
      if (Contains(tc->GetShortRomName()))
      {
         Log::Exception(StringExtensions::Build("Table with ShortRomName {0} has already been loaded (Exists more than once in ledcontrol file).", tc->GetShortRomName()));
         delete tc;
         if (throwExceptions)
            throw std::runtime_error(StringExtensions::Build("Table with ShortRomName {0} has already been loaded.", tc->GetShortRomName()));
      }
      else
      {
         Add(tc);
      }
   }
}

bool TableConfigList::Contains(const std::string& romName) const
{
   for (const TableConfig* tc : m_configs)
   {
      if (StringExtensions::ToLower(tc->GetShortRomName()) == StringExtensions::ToLower(romName))
         return true;
   }
   return false;
}

void TableConfigList::Add(TableConfig* config)
{
   if (config != nullptr)
      m_configs.push_back(config);
}

}