#include "Log.h"

namespace DOF
{

std::string Log::m_szFilename = "";

void Log::Init() {}
void Log::AfterInit() {}
void Log::Finish() {}
void Log::WriteRaw(const char* format, ...) {}

void Log::Write(const char* format, ...)
{
  char pBuffer[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(pBuffer, sizeof(pBuffer), format, args);
  printf("[INFO] %s\n", pBuffer);
  va_end(args);
}

void Log::Warning(const char* format, ...)
{
  char pBuffer[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(pBuffer, sizeof(pBuffer), format, args);
  printf("[WARNING] %s\n", pBuffer);
  va_end(args);
}

void Log::Exception(const char* format, ...) {}
void Log::Debug(const char* format, ...) {}

Log::Log() {}

}  // namespace DOF
