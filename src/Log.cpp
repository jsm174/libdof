#include "Log.h"

#include "Logger.h"

namespace DOF
{

std::string Log::m_filename = "";

void Log::Init() { }
void Log::AfterInit() { }
void Log::Finish() { }
void Log::WriteRaw(const char* format, ...) { }

void Log::Write(const char* format, ...)
{
   va_list args;
   va_start(args, format);
   ::DOF::Log(DOF_LogLevel_INFO, format, args);
   va_end(args);
}

void Log::Warning(const char* format, ...)
{
   va_list args;
   va_start(args, format);
   ::DOF::Log(DOF_LogLevel_WARN, format, args);
   va_end(args);
}

void Log::Exception(const char* format, ...)
{
   va_list args;
   va_start(args, format);
   ::DOF::Log(DOF_LogLevel_ERROR, format, args);
   va_end(args);
}

void Log::Debug(const char* format, ...)
{
   va_list args;
   va_start(args, format);
   ::DOF::Log(DOF_LogLevel_DEBUG, format, args);
   va_end(args);
}

Log::Log() { }

} // namespace DOF
