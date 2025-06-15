#include "DirectoryInfo.h"

#include <regex>
#include <unordered_set>

namespace DOF
{

DirectoryInfo::DirectoryInfo(const std::string& path)
   : m_path(path)
{
}

std::string DirectoryInfo::Name() const { return m_path.filename().string(); }

std::string DirectoryInfo::FullName() const { return m_path.string(); }

bool DirectoryInfo::Exists() const { return std::filesystem::exists(m_path) && std::filesystem::is_directory(m_path); }

bool DirectoryInfo::Create() const
{
   std::error_code ec;
   bool ok = std::filesystem::create_directory(m_path, ec);
   return ok && !ec;
}

bool DirectoryInfo::Delete(bool recursive) const
{
   std::error_code ec;
   if (recursive)
      std::filesystem::remove_all(m_path, ec);
   else
      std::filesystem::remove(m_path, ec);
   return !ec;
}

std::vector<FileInfo> DirectoryInfo::GetFiles(const std::string& pattern) const
{
   std::vector<FileInfo> result;
   if (!Exists())
      return result;
   std::regex rx(WildcardToRegex(pattern), std::regex::ECMAScript | std::regex::icase);
   for (auto const& entry : std::filesystem::directory_iterator(m_path))
   {
      if (entry.is_regular_file())
      {
         auto name = entry.path().filename().string();
         if (std::regex_match(name, rx))
            result.emplace_back(entry.path().string());
      }
   }
   return result;
}

std::vector<FileInfo> DirectoryInfo::EnumerateFiles(const std::string& pattern) const { return GetFiles(pattern); }

std::vector<DirectoryInfo> DirectoryInfo::GetDirectories(const std::string& pattern) const
{
   std::vector<DirectoryInfo> result;
   if (!Exists())
      return result;
   std::regex rx(WildcardToRegex(pattern), std::regex::ECMAScript | std::regex::icase);
   for (auto const& entry : std::filesystem::directory_iterator(m_path))
   {
      if (entry.is_directory())
      {
         auto name = entry.path().filename().string();
         if (std::regex_match(name, rx))
            result.emplace_back(entry.path().string());
      }
   }
   return result;
}

std::string DirectoryInfo::WildcardToRegex(const std::string& wildcard)
{
   std::string s;
   s.reserve(wildcard.size() * 2);
   s.push_back('^');
   for (auto c : wildcard)
   {
      switch (c)
      {
      case '*': s.append(".*"); break;
      case '?': s.push_back('.'); break;
      case '.': s.append("\\."); break;
      case '\\': s.append("\\\\"); break;
      default:
         if (std::isalnum(static_cast<unsigned char>(c)))
            s.push_back(c);
         else
         {
            s.push_back('\\');
            s.push_back(c);
         }
      }
   }
   s.push_back('$');
   return s;
}

bool DirectoryInfo::CreateDirectoryPath() const
{
   std::error_code ec;
   std::filesystem::create_directories(m_path, ec);
   return !ec;
}

}