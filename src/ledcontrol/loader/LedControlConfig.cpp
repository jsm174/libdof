#include "LedControlConfig.h"
#include "TableConfig.h"
#include "TableConfigColumn.h"
#include "TableConfigColumnList.h"
#include "TableConfigSetting.h"
#include "ColorConfig.h"
#include "TableVariablesDictionary.h"
#include "VariablesDictionary.h"
#include "../../Log.h"
#include "../../general/FileReader.h"
#include "../../general/StringExtensions.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

namespace DOF
{

LedControlConfig::LedControlConfig()
   : m_ledWizNumber(0)
   , m_tableConfigurations(new TableConfigList())
   , m_colorConfigurations(new ColorConfigList())
{
}

LedControlConfig::LedControlConfig(const std::string& ledControlIniFilename, int ledWizNumber, bool throwExceptions)
   : m_ledWizNumber(ledWizNumber)
   , m_tableConfigurations(new TableConfigList())
   , m_colorConfigurations(new ColorConfigList())
{
   ParseLedControlIni(ledControlIniFilename, throwExceptions);
}

LedControlConfig::~LedControlConfig()
{
   delete m_tableConfigurations;
   delete m_colorConfigurations;
}

void LedControlConfig::ParseLedControlIni(const std::string& ledControlIniFilename, bool throwExceptions)
{
   std::vector<std::string> colorStartStrings = { "[Colors DOF]", "[Colors LedWiz]" };
   std::vector<std::string> outStartStrings = { "[Config DOF]", "[Config outs]" };
   std::vector<std::string> variableStartStrings = { "[Variables DOF]" };
   std::vector<std::string> versionStartStrings = { "[version]" };
   std::vector<std::string> tableVariableStartStrings = { "[TableVariables]" };

   std::string fileData;

   try
   {
      fileData = FileReader::ReadFileToString(ledControlIniFilename);
   }
   catch (const std::runtime_error& e)
   {
      Log::Exception(StringExtensions::Build("Could not read file {0}.", ledControlIniFilename));
      if (throwExceptions)
         throw std::runtime_error(StringExtensions::Build("Could not read file {0}.", ledControlIniFilename));
      return;
   }

   if (StringExtensions::IsNullOrWhiteSpace(fileData))
   {
      Log::Warning(StringExtensions::Build("File {0} does not contain data.", ledControlIniFilename));
      if (throwExceptions)
         throw std::runtime_error(StringExtensions::Build("File {0} does not contain data.", ledControlIniFilename));
      return;
   }

   std::unordered_map<std::string, std::vector<std::string>> sections;
   std::vector<std::string> sectionData;
   std::string sectionHeader;

   std::vector<std::string> lines = StringExtensions::Split(fileData, { '\r', '\n' });

   for (const std::string& rawIniLine : lines)
   {
      std::string iniLine = StringExtensions::Trim(rawIniLine);
      if (iniLine.length() > 0 && !StringExtensions::StartsWith(iniLine, "#"))
      {
         if (StringExtensions::StartsWith(iniLine, "[") && StringExtensions::EndsWith(iniLine, "]") && iniLine.length() > 2)
         {

            if (!StringExtensions::IsNullOrWhiteSpace(sectionHeader))
            {
               if (sections.find(sectionHeader) != sections.end())
               {
                  int cnt = 2;
                  std::string newSectionHeader = StringExtensions::Build("{0} {1}", sectionHeader, std::to_string(cnt));
                  while (sections.find(newSectionHeader) != sections.end())
                  {
                     cnt++;
                     if (cnt > 999)
                        throw std::runtime_error(StringExtensions::Build("Section header {0} exists to many times.", sectionHeader));
                     newSectionHeader = StringExtensions::Build("{0} {1}", sectionHeader, std::to_string(cnt));
                  }
                  sectionHeader = newSectionHeader;
               }
               sections[sectionHeader] = sectionData;
               sectionData.clear();
            }
            sectionHeader = iniLine;
         }
         else
         {

            sectionData.push_back(iniLine);
         }
      }
   }

   if (!StringExtensions::IsNullOrWhiteSpace(sectionHeader))
   {
      if (sections.find(sectionHeader) != sections.end())
      {
         int cnt = 2;
         std::string newSectionHeader = StringExtensions::Build("{0} {1}", sectionHeader, std::to_string(cnt));
         while (sections.find(newSectionHeader) != sections.end())
         {
            cnt++;
            if (cnt > 999)
               throw std::runtime_error(StringExtensions::Build("Section header {0} exists to many times.", sectionHeader));
            newSectionHeader = StringExtensions::Build("{0} {1}", sectionHeader, std::to_string(cnt));
         }
         sectionHeader = newSectionHeader;
      }
      sections[sectionHeader] = sectionData;
   }

   fileData.clear();

   std::vector<std::string> colorData = GetSection(sections, colorStartStrings);
   std::vector<std::string> outData = GetSection(sections, outStartStrings);
   std::vector<std::string> variableData = GetSection(sections, variableStartStrings);
   std::vector<std::string> versionData = GetSection(sections, versionStartStrings);
   std::vector<std::string> tableVariableData = GetSection(sections, tableVariableStartStrings);

   if (versionData.size() > 0)
   {
      m_minDOFVersion.clear();

      std::string minDofVersionLine;
      for (const std::string& s : versionData)
      {
         if (StringExtensions::StartsWith(StringExtensions::ToLower(s), "mindofversion="))
         {
            minDofVersionLine = s;
            break;
         }
      }

      if (!minDofVersionLine.empty())
      {
         std::string minDofVersionString = minDofVersionLine.substr(std::string("mindofversion=").length());

         try
         {
            m_minDOFVersion = minDofVersionString;
         }
         catch (const std::exception&)
         {
            Log::Exception(StringExtensions::Build("Could not parse line {1} from file {0}", ledControlIniFilename, minDofVersionLine));
            m_minDOFVersion.clear();
         }
         if (!m_minDOFVersion.empty())
         {
            Log::Write(StringExtensions::Build("Min DOF Version is {0} for file {1}", m_minDOFVersion, ledControlIniFilename));
         }
      }
      else
      {
         Log::Warning(StringExtensions::Build("No version section found in file {0}.", ledControlIniFilename));
      }
   }


   if (colorData.empty())
   {
      Log::Warning(StringExtensions::Build("Could not find color definition section in file {0}.", ledControlIniFilename));
      if (throwExceptions)
         throw std::runtime_error(StringExtensions::Build("Could not find color definition section in file {0}.", ledControlIniFilename));
      return;
   }

   if (outData.empty())
   {
      Log::Warning(StringExtensions::Build("File {0} does not contain data in the table config section.", ledControlIniFilename));
      if (throwExceptions)
         throw std::runtime_error(StringExtensions::Build("File {0} does not contain data in the table config section.", ledControlIniFilename));
      return;
   }


   if (!tableVariableData.empty())
   {
      Log::Write("Resolving Table Variables");
      ResolveTableVariables(outData, tableVariableData);
   }

   if (!variableData.empty())
   {
      Log::Write("Resolving Global Variables");
      ResolveVariables(outData, variableData);
   }

   m_colorConfigurations->ParseLedControlData(colorData, throwExceptions);

   m_tableConfigurations->ParseLedcontrolData(outData, throwExceptions);

   ResolveRGBColors();

   m_ledControlIniFile = ledControlIniFilename;
}

std::vector<std::string> LedControlConfig::GetSection(const std::unordered_map<std::string, std::vector<std::string>>& sections, const std::vector<std::string>& sectionStartStrings)
{
   for (const std::string& startString : sectionStartStrings)
   {
      auto it = sections.find(startString);
      if (it != sections.end())
         return it->second;
   }
   return std::vector<std::string>();
}

void LedControlConfig::ResolveTableVariables(std::vector<std::string>& dataToResolve, const std::vector<std::string>& variableData)
{
   TableVariablesDictionary vd(variableData);

   for (size_t i = 0; i < dataToResolve.size(); i++)
   {
      std::string d = StringExtensions::Trim(dataToResolve[i]);
      bool updated = false;
      if (!StringExtensions::IsNullOrWhiteSpace(d))
      {
         size_t tp = d.find(",");
         if (tp != std::string::npos && tp > 0)
         {
            std::string tableName = StringExtensions::Trim(d.substr(0, tp));
            if (vd.find(tableName) != vd.end())
            {
               for (const auto& kv : vd[tableName])
               {
                  std::string n = kv.first;
                  if (!StringExtensions::StartsWith(n, "@"))
                     n = "@" + n;
                  if (!StringExtensions::EndsWith(n, "@"))
                     n += "@";
                  d = StringExtensions::Replace(d, n, kv.second);
                  updated = true;
               }
            }
         }
      }
      if (updated)
         dataToResolve[i] = d;
   }
}

void LedControlConfig::ResolveVariables(std::vector<std::string>& dataToResolve, const std::vector<std::string>& variableData)
{
   VariablesDictionary vd(variableData);

   for (const auto& kv : vd)
   {
      std::string n = kv.first;
      if (!StringExtensions::StartsWith(n, "@"))
         n = "@" + n;
      if (!StringExtensions::EndsWith(n, "@"))
         n += "@";

      for (size_t i = 0; i < dataToResolve.size(); i++)
         dataToResolve[i] = StringExtensions::Replace(dataToResolve[i], n, kv.second);
   }
}

void LedControlConfig::ResolveRGBColors()
{
   for (size_t i = 0; i < m_tableConfigurations->Size(); i++)
   {
      TableConfig* tc = (*m_tableConfigurations)[i];
      for (TableConfigColumn* c : *tc->GetColumns())
      {
         for (TableConfigSetting* s : *c)
         {
            for (size_t j = 0; j < m_colorConfigurations->Size(); j++)
            {
               ColorConfig* cc = (*m_colorConfigurations)[j];
               if (StringExtensions::ToLower(cc->GetName()) == StringExtensions::ToLower(s->GetColorName()))
               {
                  s->SetColorConfig(cc);
                  break;
               }
            }
         }
      }
   }
}

}