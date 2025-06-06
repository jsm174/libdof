#pragma once

#include <string>

namespace DOF
{

class FileReader
{
public:
   static std::string ReadFileToString(const std::string& filename);
};

} // namespace DOF
