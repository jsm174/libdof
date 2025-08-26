#pragma once

#include "DOF/DOF.h"
#include <unordered_set>

namespace DOF
{

class Log
{
public:
   static const std::string& GetFilename();
   static void SetFilename(const std::string& filename) { m_filename = filename; }

   static void Init();
   static void AfterInit();
   static void Finish();
   static void WriteRaw(const char* format, ...);
   static void Write(const std::string& message);
   static void Warning(const std::string& message);
   static void Error(const std::string& message);
   static void Exception(const std::string& message);
   static void Debug(const std::string& message);
   static void Once(const std::string& key, const std::string& message);
   static void Instrumentation(const std::string& key, const std::string& message);

private:
   Log();
   ~Log() { };

   static std::string m_filename;
   static std::unordered_set<std::string> m_onceKeys;
};

}
