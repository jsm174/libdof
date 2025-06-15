#include "TableVariablesDictionary.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"

namespace DOF
{

TableVariablesDictionary::TableVariablesDictionary() { }

TableVariablesDictionary::TableVariablesDictionary(const std::vector<std::string>& dataToParse)
{
   for (const std::string& d : dataToParse)
   {
      size_t tp = d.find(",");
      if (tp != std::string::npos && tp > 0)
      {
         std::string tableName = StringExtensions::Trim(d.substr(0, tp));
         if (find(tableName) == end())
            (*this)[tableName] = VariablesDictionary();
         std::string vd = StringExtensions::Trim(d.substr(tp + 1));

         size_t pos = 0;
         size_t lastPos = 0;
         while (pos < vd.length())
         {
            pos = vd.find('=', lastPos);
            if (pos == std::string::npos)
               break;
            if (pos - lastPos < 1)
            {
               Log::Warning(StringExtensions::Build("Will skip some variable definitions due to missing variable name before = in line {0}", d));
               break;
            }
            std::string varName = StringExtensions::Trim(vd.substr(lastPos, pos - lastPos));
            std::string value = "";

            pos++;
            if (pos < vd.length())
            {
               lastPos = pos;

               if (vd[pos] == '{')
               {
                  pos = vd.find('}', lastPos);
                  if (pos == std::string::npos)
                  {
                     Log::Warning(StringExtensions::Build("Will skip some variable definitions due to missing closing } bracket in line {0}", d));
                     break;
                  }
                  value = vd.substr(lastPos + 1, pos - lastPos - 1);

                  pos++;
                  while (pos < vd.length() && vd[pos] == ' ')
                     pos++;
                  if (pos < vd.length())
                  {
                     if (vd[pos] != '/')
                     {
                        Log::Warning(StringExtensions::Build("Will skip some variable definitions due to missing / after } in line {0}", d));
                        break;
                     }
                  }
               }
               else
               {
                  pos = vd.find('/', lastPos);
                  if (pos == std::string::npos)
                     pos = vd.length();
                  value = StringExtensions::Trim(vd.substr(lastPos, pos - lastPos));
               }
            }
            if ((*this)[tableName].find(varName) == (*this)[tableName].end())
               (*this)[tableName][varName] = value;
            else
            {
               Log::Warning(StringExtensions::Build("Variable {0} has been defined more than once in line {1}.", varName, d));
            }

            lastPos = pos + 1;
         }
      }
      else
      {
         Log::Warning(StringExtensions::Build("Could not find comma in TableVariables section line {0}.", d));
      }
   }
}

}