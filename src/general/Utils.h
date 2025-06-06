#pragma once

#include <string>
#include <ranges>
#include <charconv>
#include <filesystem>
#include <fstream>

namespace DOF
{

static void WriteToFile(const std::string& data, const std::string& filename, bool append)
{
   std::ofstream ofs;
   ofs.exceptions(std::ios::failbit | std::ios::badbit);
   ofs.open(filename, append ? (std::ios::out | std::ios::app) : std::ios::out);
   ofs << data;
}

static void WriteToFile(const std::string& data, const std::string& filename) { WriteToFile(data, filename, false); }


static std::string GetFileNameWithoutExtension(const std::string& fullPath) { return std::filesystem::path(fullPath).stem().string(); }

static bool TryParseInt(const std::string& str, int& value) { return (std::from_chars(str.c_str(), str.c_str() + str.length(), value).ec == std::errc {}); }

static bool IsInteger(const std::string& s) { return std::all_of(s.begin(), s.end(), ::isdigit); }

static std::string ToLower(const std::string& s)
{
   std::string result = s;
   std::ranges::transform(result, result.begin(), [](unsigned char c) { return std::tolower(c); });
   return result;
}

static bool IsNullOrWhiteSpace(const std::string& s)
{
   return s.empty() || std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isspace(c); });
}

} // namespace DOF