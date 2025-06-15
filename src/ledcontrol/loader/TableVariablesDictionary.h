#pragma once

#include "VariablesDictionary.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace DOF
{

class TableVariablesDictionary : public std::unordered_map<std::string, VariablesDictionary>
{
public:
   TableVariablesDictionary();
   TableVariablesDictionary(const std::vector<std::string>& dataToParse);
};

}