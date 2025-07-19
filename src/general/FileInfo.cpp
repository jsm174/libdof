#include "FileInfo.h"

#include "DirectoryInfo.h"

namespace DOF
{

FileInfo::FileInfo(const std::string& path)
   : m_path(path)
{
}

bool FileInfo::Exists() const { return std::filesystem::exists(m_path); }

uintmax_t FileInfo::Length() const { return std::filesystem::file_size(m_path); }

std::string FileInfo::Name() const { return m_path.filename().string(); }

std::string FileInfo::FullName() const { return m_path.string(); }

std::string FileInfo::Extension() const { return m_path.extension().string(); }

bool FileInfo::Delete() const { return std::filesystem::remove(m_path); }

DirectoryInfo FileInfo::Directory() const { return DirectoryInfo(m_path.parent_path().string()); }

}
