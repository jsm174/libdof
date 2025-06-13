#include "Logger.h"

#include "DOF/Config.h"

namespace DOF
{

void Log(DOF_LogLevel logLevel, const char* format, ...)
{
   va_list args;
   va_start(args, format);
   Log(logLevel, format, args);
   va_end(args);
}

void Log(DOF_LogLevel logLevel, const char* format, va_list args)
{
   static Config* pConfig = Config::GetInstance();
   auto callback = pConfig->GetLogCallback();
   if (!callback || logLevel > pConfig->GetLogLevel())
      return;
   callback(logLevel, format, args);
}

}
