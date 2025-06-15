#pragma once

#include <unordered_map>
#include <string>
#include <vector>

namespace DOF
{

class VariablesDictionary : public std::unordered_map<std::string, std::string>
{
public:
   VariablesDictionary();
   VariablesDictionary(const std::vector<std::string>& dataToParse);
};

}