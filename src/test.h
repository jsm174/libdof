#include <chrono>
#include <cstring>
#include <thread>

#include "DOF/DOF.h"

void LIBDOFCALLBACK LogCallback(const char* format, va_list args)
{
  char buffer[1024];
  vsnprintf(buffer, sizeof(buffer), format, args);
  printf("%s\n", buffer);
}

void run(DOF::DOF* pDof) { DOF::Config* pConfig = DOF::Config::GetInstance(); }
