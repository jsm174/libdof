#include "LedControlConfigList.h"
#include "LedControlConfig.h"
#include "TableConfig.h"
#include "TableConfigList.h"
#include "../../Log.h"
#include "../../general/FileInfo.h"
#include "../../general/StringExtensions.h"

#include <algorithm>

namespace DOF
{

LedControlConfigList::LedControlConfigList() { }

LedControlConfigList::LedControlConfigList(const std::vector<std::string>& ledControlFilenames, bool throwExceptions) { LoadLedControlFiles(ledControlFilenames, throwExceptions); }

LedControlConfigList::~LedControlConfigList()
{
   for (LedControlConfig* config : *this)
      delete config;

   clear();
}

std::unordered_map<int, TableConfig*> LedControlConfigList::GetTableConfigDictionary(const std::string& romName)
{
   std::unordered_map<int, TableConfig*> d;

   bool foundMatch = false;
   for (LedControlConfig* lcc : *this)
   {
      for (size_t i = 0; i < lcc->GetTableConfigurations()->Size(); i++)
      {
         TableConfig* tc = (*lcc->GetTableConfigurations())[i];
         if (StringExtensions::ToUpper(romName) == StringExtensions::ToUpper(tc->GetShortRomName()))
         {
            d[lcc->GetLedWizNumber()] = tc;
            foundMatch = true;
            break;
         }
      }
   }

   if (foundMatch)
      return d;

   for (LedControlConfig* lcc : *this)
   {
      for (size_t i = 0; i < lcc->GetTableConfigurations()->Size(); i++)
      {
         TableConfig* tc = (*lcc->GetTableConfigurations())[i];
         if (StringExtensions::StartsWith(StringExtensions::ToUpper(romName), StringExtensions::Build("{0}_", StringExtensions::ToUpper(tc->GetShortRomName()))))
         {
            d[lcc->GetLedWizNumber()] = tc;
            foundMatch = true;
            break;
         }
      }
   }

   if (foundMatch)
      return d;

   for (LedControlConfig* lcc : *this)
   {
      for (size_t i = 0; i < lcc->GetTableConfigurations()->Size(); i++)
      {
         TableConfig* tc = (*lcc->GetTableConfigurations())[i];
         if (StringExtensions::StartsWith(romName, tc->GetShortRomName()))
         {
            d[lcc->GetLedWizNumber()] = tc;
            break;
         }
      }
   }
   return d;
}

bool LedControlConfigList::ContainsConfig(const std::string& romName) { return GetTableConfigDictionary(romName).size() > 0; }

void LedControlConfigList::LoadLedControlFiles(const std::vector<std::string>& ledControlFilenames, bool throwExceptions)
{
   for (int i = 0; i < static_cast<int>(ledControlFilenames.size()); i++)
      LoadLedControlFile(ledControlFilenames[i], i + 1, throwExceptions);
}

void LedControlConfigList::LoadLedControlFiles(const std::unordered_map<int, FileInfo>& ledControlIniFiles, bool throwExceptions)
{
   for (const auto& f : ledControlIniFiles)
      LoadLedControlFile(f.second.FullName(), f.first, throwExceptions);
}

void LedControlConfigList::LoadLedControlFile(const std::string& ledControlFilename, int ledWizNumber, bool throwExceptions)
{
   Log::Write(StringExtensions::Build("Loading LedControl file {0}", ledControlFilename));

   LedControlConfig* lcc = new LedControlConfig(ledControlFilename, ledWizNumber, throwExceptions);
   push_back(lcc);
}

}