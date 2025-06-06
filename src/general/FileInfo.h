#pragma once

#include <filesystem>
#include <string>

namespace DOF
{

class DirectoryInfo;

class FileInfo
{
public:
   FileInfo(const std::string& path);
   bool Exists() const;
   uintmax_t Length() const;
   std::string Name() const;
   std::string FullName() const;
   std::string Extension() const;
   bool Delete() const;
   DirectoryInfo* Directory() const;

private:
   std::filesystem::path m_path;
};

} // namespace DOF