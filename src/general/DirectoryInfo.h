#pragma once

#include "FileInfo.h"

#include <filesystem>
#include <string>
#include <vector>

namespace DOF
{

class DirectoryInfo
{
public:
   DirectoryInfo(const std::string& path);
   std::string Name() const;
   std::string FullName() const;
   bool Exists() const;
   bool Create() const;
   bool Delete(bool recursive = false) const;
   std::vector<FileInfo> GetFiles(const std::string& pattern = "*") const;
   std::vector<FileInfo> EnumerateFiles(const std::string& pattern = "*") const;
   std::vector<DirectoryInfo> GetDirectories(const std::string& pattern = "*") const;
   static std::string WildcardToRegex(const std::string& wildcard);
   bool CreateDirectoryPath() const;

private:
   std::filesystem::path m_path;
};

}