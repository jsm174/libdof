#include "Logger.h"

#include "DOF/Config.h"

namespace DOF
{

void Log(const char *format, ...)
{
  DOF_LogCallback logCallback = Config::GetInstance()->GetLogCallback();

  if (!logCallback) return;

  va_list args;
  va_start(args, format);
  (*(logCallback))(format, args);
  va_end(args);
}

}  // namespace DOF
