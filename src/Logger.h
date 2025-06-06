#pragma once

#include "DOF/Config.h"

namespace DOF
{

void Log(DOF_LogLevel logLevel, const char* format, ...);
void Log(DOF_LogLevel logLevel, const char* format, va_list args);

} // namespace DOF
