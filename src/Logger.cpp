#include "Logger.h"

#include "DOF/Config.h"

namespace DOF
{

void Log(DOF_LogLevel logLevel, const char* format, ...)
{
  static Config* pConfig = Config::GetInstance();

  DOF_LogCallback logCallback = pConfig->GetLogCallback();

  if (!logCallback || logLevel < pConfig->GetLogLevel()) return;

  va_list args;
  va_start(args, format);
  (*(logCallback))(logLevel, format, args);
  va_end(args);
}

}  // namespace DOF
