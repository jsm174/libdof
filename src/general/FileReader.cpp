#include "FileReader.h"

#include <filesystem>
#include <fstream>
#include <iterator>

namespace DOF
{

std::string FileReader::ReadFileToString(const std::string& filename)
{
   if (!std::filesystem::exists(filename))
      throw std::runtime_error("File not found: " + filename);

   std::ifstream in(filename, std::ios::binary);
   in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

   std::string contents;
   in.seekg(0, std::ios::end);
   contents.resize(static_cast<size_t>(in.tellg()));
   in.seekg(0, std::ios::beg);
   in.read(&contents[0], contents.size());

   return contents;
}

}