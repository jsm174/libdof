#include "Log.h"

#include "Logger.h"

namespace DOF
{

std::string Log::m_filename = "";

void Log::Init() { }
void Log::AfterInit() { }
void Log::Finish() { }
void Log::WriteRaw(const char* format, ...) { }

void Log::Write(const std::string& message) { ::DOF::Log(DOF_LogLevel_INFO, "%s", message.c_str()); }

void Log::Warning(const std::string& message) { ::DOF::Log(DOF_LogLevel_WARN, "%s", message.c_str()); }

void Log::Exception(const std::string& message) { ::DOF::Log(DOF_LogLevel_ERROR, "%s", message.c_str()); }

void Log::Debug(const std::string& message) { ::DOF::Log(DOF_LogLevel_DEBUG, "%s", message.c_str()); }

Log::Log() { }

}
