#include "VariablesDictionary.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"

namespace DOF
{

VariablesDictionary::VariablesDictionary() { }

VariablesDictionary::VariablesDictionary(const std::vector<std::string>& dataToParse)
{
   for (const std::string& d : dataToParse)
   {
      size_t p = d.find("=");
      if (p != std::string::npos && p > 0)
      {
         std::string key = StringExtensions::Trim(d.substr(0, p));
         std::string value = StringExtensions::Trim(d.substr(p + 1));
         (*this)[key] = value;
      }
      else
      {
         Log::Warning(StringExtensions::Build("Could not find = in variables section line {0}.", d));
      }
   }
}

}