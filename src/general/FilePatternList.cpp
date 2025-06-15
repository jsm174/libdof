#include "FilePatternList.h"

#include <unordered_set>

namespace DOF
{

std::vector<FileInfo> FilePatternList::GetMatchingFiles(const std::unordered_map<std::string, std::string>& replaceValues) const
{
   std::vector<FileInfo> result;
   std::unordered_set<std::string> seen;
   for (auto const& pattern : *this)
   {
      auto files = pattern.GetMatchingFiles(replaceValues);
      for (auto const& fi : files)
      {
         auto full = fi.FullName();
         if (seen.insert(full).second)
            result.emplace_back(full);
      }
   }
   return result;
}

FileInfo* FilePatternList::GetFirstMatchingFile(const std::unordered_map<std::string, std::string>& replaceValues) const
{
   for (auto const& pattern : *this)
   {
      if (auto fi = pattern.GetFirstMatchingFile(replaceValues))
         return fi;
   }
   return nullptr;
}

}