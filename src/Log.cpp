#include "Log.h"

#include "Logger.h"

namespace DOF
{

std::string Log::m_filename = "";
std::unordered_set<std::string> Log::m_onceKeys = {};

void Log::Init() { }
void Log::AfterInit() { }
void Log::Finish() { }
void Log::WriteRaw(const char* format, ...) { }

void Log::Write(const std::string& message) { ::DOF::Log(DOF_LogLevel_INFO, "%s", message.c_str()); }

void Log::Warning(const std::string& message) { ::DOF::Log(DOF_LogLevel_WARN, "%s", message.c_str()); }

void Log::Exception(const std::string& message) { ::DOF::Log(DOF_LogLevel_ERROR, "%s", message.c_str()); }

void Log::Debug(const std::string& message) { ::DOF::Log(DOF_LogLevel_DEBUG, "%s", message.c_str()); }

void Log::Once(const std::string& key, const std::string& message)
{
   if (m_onceKeys.find(key) == m_onceKeys.end())
   {
      m_onceKeys.insert(key);
      Write(message);
   }
}

void Log::Instrumentation(const std::string& key, const std::string& message) { Debug(message); }

Log::Log() { }

}
