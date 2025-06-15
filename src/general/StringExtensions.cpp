#include "StringExtensions.h"

namespace DOF
{

std::string StringExtensions::ToAddressString(const void* ptr)
{
   std::ostringstream oss;
   oss << "0x" << std::uppercase << std::hex << reinterpret_cast<std::uintptr_t>(ptr);
   return oss.str();
}

int StringExtensions::ToInteger(const std::string& s)
{
   int value = 0;
   TryParseInt(s, value);
   return value;
}

bool StringExtensions::IsInteger(const std::string& s)
{
   if (s.empty())
      return false;


   size_t start = 0;
   if (s[0] == '-')
      start = 1;

   return std::all_of(s.begin() + start, s.end(), [](unsigned char c) { return std::isdigit(c); });
}

bool StringExtensions::IsNullOrEmpty(const std::string& s) { return s.empty(); }

bool StringExtensions::IsNullOrWhiteSpace(const std::string& s)
{
   return s.empty() || std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isspace(c); });
}

std::string StringExtensions::ToUpper(const std::string& s)
{
   std::string result = s;
   std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
   return result;
}

std::string StringExtensions::ToLower(const std::string& s)
{
   std::string result = s;
   std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
   return result;
}

std::string StringExtensions::Trim(const std::string& s)
{
   if (s.empty())
      return s;

   size_t start = 0;
   size_t end = s.length();

   while (start < end && std::isspace(static_cast<unsigned char>(s[start])))
      start++;

   while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
      end--;

   return s.substr(start, end - start);
}

bool StringExtensions::StartsWith(const std::string& s, const std::string& prefix)
{
   if (prefix.length() > s.length())
      return false;
   return s.substr(0, prefix.length()) == prefix;
}

bool StringExtensions::EndsWith(const std::string& s, const std::string& suffix)
{
   if (suffix.length() > s.length())
      return false;
   return s.substr(s.length() - suffix.length()) == suffix;
}

std::string StringExtensions::Replace(const std::string& s, const std::string& from, const std::string& to)
{
   if (from.empty())
      return s;

   std::string result = s;
   size_t pos = 0;
   while ((pos = result.find(from, pos)) != std::string::npos)
   {
      result.replace(pos, from.length(), to);
      pos += to.length();
   }
   return result;
}

std::vector<std::string> StringExtensions::Split(const std::string& s, const std::vector<char>& delimiters)
{
   std::vector<std::string> result;
   if (s.empty())
      return result;

   size_t start = 0;
   size_t pos = 0;

   while (pos < s.length())
   {
      bool found = false;
      for (char delimiter : delimiters)
      {
         if (s[pos] == delimiter)
         {
            found = true;
            break;
         }
      }

      if (found)
      {
         if (pos > start)
         {
            result.push_back(s.substr(start, pos - start));
         }
         start = pos + 1;
      }
      pos++;
   }

   if (start < s.length())
   {
      result.push_back(s.substr(start));
   }

   return result;
}

void StringExtensions::WriteToFile(const std::string& data, const std::string& filename) { WriteToFile(data, filename, false); }

void StringExtensions::WriteToFile(const std::string& data, const std::string& filename, bool append)
{
   std::ofstream ofs;
   ofs.exceptions(std::ios::failbit | std::ios::badbit);
   ofs.open(filename, append ? (std::ios::out | std::ios::app) : std::ios::out);
   ofs << data;
}

std::string FormatArgument(const std::string& arg, const std::string& formatSpec)
{
   if (formatSpec.empty())
   {
      return arg;
   }


   if (formatSpec.length() > 0 && formatSpec[0] == '0')
   {
      int width = formatSpec.length();
      if (arg.length() < width)
      {
         return std::string(width - arg.length(), '0') + arg;
      }
   }

   return arg;
}

std::string ReplaceArgument(const std::string& format, int argIndex, const std::string& arg)
{
   std::string result = format;
   std::string pattern = "{" + std::to_string(argIndex);

   size_t pos = result.find(pattern);
   if (pos != std::string::npos)
   {
      size_t endPos = result.find("}", pos);
      if (endPos != std::string::npos)
      {
         std::string placeholder = result.substr(pos, endPos - pos + 1);
         std::string formattedArg = arg;


         size_t colonPos = placeholder.find(":");
         if (colonPos != std::string::npos)
         {
            std::string formatSpec = placeholder.substr(colonPos + 1, placeholder.length() - colonPos - 2);
            formattedArg = FormatArgument(arg, formatSpec);
         }

         result.replace(pos, endPos - pos + 1, formattedArg);
      }
   }
   return result;
}

std::string StringExtensions::Build(const std::string& format, const std::string& arg0) { return ReplaceArgument(format, 0, arg0); }

std::string StringExtensions::Build(const std::string& format, const std::string& arg0, const std::string& arg1)
{
   std::string result = ReplaceArgument(format, 0, arg0);
   result = ReplaceArgument(result, 1, arg1);
   return result;
}

std::string StringExtensions::Build(const std::string& format, const std::string& arg0, const std::string& arg1, const std::string& arg2)
{
   std::string result = ReplaceArgument(format, 0, arg0);
   result = ReplaceArgument(result, 1, arg1);
   result = ReplaceArgument(result, 2, arg2);
   return result;
}

std::string StringExtensions::Build(const std::string& format, const std::string& arg0, const std::string& arg1, const std::string& arg2, const std::string& arg3)
{
   std::string result = ReplaceArgument(format, 0, arg0);
   result = ReplaceArgument(result, 1, arg1);
   result = ReplaceArgument(result, 2, arg2);
   result = ReplaceArgument(result, 3, arg3);
   return result;
}

std::string StringExtensions::GetFileNameWithoutExtension(const std::string& fullPath) { return std::filesystem::path(fullPath).stem().string(); }

bool StringExtensions::TryParseInt(const std::string& str, int& value)
{
   if (str.empty())
      return false;
   return (std::from_chars(str.c_str(), str.c_str() + str.length(), value).ec == std::errc {});
}

bool StringExtensions::IsHexString(const std::string& s)
{
   if (s.empty())
      return false;
   return std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'); });
}

int StringExtensions::HexToInt(const std::string& s)
{
   int result;
   std::istringstream iss(s);
   iss >> std::hex >> result;
   return result;
}

}