#pragma once

#include "DOF/DOF.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace DOF
{

class LedControlConfig;
class FileInfo;
class TableConfig;

class LedControlConfigList : public std::vector<LedControlConfig*>
{
public:
   std::unordered_map<int, TableConfig*> GetTableConfigDictionary(const std::string& romName);
   bool ContainsConfig(const std::string& romName);
   void LoadLedControlFiles(const std::vector<std::string>& ledControlFilenames, bool throwExceptions = false);
   void LoadLedControlFiles(const std::unordered_map<int, FileInfo>& ledControlIniFiles, bool throwExceptions = false);
   void LoadLedControlFile(const std::string& ledControlFilename, int ledWizNumber, bool throwExceptions = false);
   LedControlConfigList();
   LedControlConfigList(const std::vector<std::string>& ledControlFilenames, bool throwExceptions = false);

   ~LedControlConfigList();
};

}
