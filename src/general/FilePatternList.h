#pragma once

#include "FilePattern.h"

#include <unordered_map>
#include <vector>

namespace DOF
{

class FilePatternList : public std::vector<FilePattern>
{
public:
   std::vector<FileInfo> GetMatchingFiles(const std::unordered_map<std::string, std::string>& replaceValues = {}) const;
   FileInfo* GetFirstMatchingFile(const std::unordered_map<std::string, std::string>& replaceValues = {}) const;
};

}
