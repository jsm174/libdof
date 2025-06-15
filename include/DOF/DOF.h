#pragma once

#define LIBDOF_VERSION_MAJOR 0 // X Digits
#define LIBDOF_VERSION_MINOR 2 // Max 2 Digits
#define LIBDOF_VERSION_PATCH 0 // Max 2 Digits

#define _LIBDOF_STR(x) #x
#define LIBDOF_STR(x) _LIBDOF_STR(x)

#define LIBDOF_VERSION LIBDOF_STR(LIBDOF_VERSION_MAJOR) "." LIBDOF_STR(LIBDOF_VERSION_MINOR) "." LIBDOF_STR(LIBDOF_VERSION_PATCH)
#define LIBDOF_MINOR_VERSION LIBDOF_STR(LIBDOF_VERSION_MAJOR) "." LIBDOF_STR(LIBDOF_VERSION_MINOR)

#ifdef _MSC_VER
#define LIBDOFAPI __declspec(dllexport)
#define LIBDOFCALLBACK __stdcall
#else
#define LIBDOFAPI __attribute__((visibility("default")))
#define LIBDOFCALLBACK
#endif

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <string>
#include <thread>

#include "Config.h"

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#if !((defined(__APPLE__) && ((defined(TARGET_OS_IOS) && TARGET_OS_IOS) || (defined(TARGET_OS_TV) && TARGET_OS_TV))) || defined(__ANDROID__))
#define __HIDAPI__
#define __LIBSERIALPORT__
#endif

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#if !defined(PATH_SEPARATOR_CHAR)
#ifdef _MSC_VER
#define PATH_SEPARATOR_CHAR '\\'
#else
#define PATH_SEPARATOR_CHAR '/'
#endif
#endif

namespace DOF
{

class Pinball;

class LIBDOFAPI DOF
{
public:
   DOF();
   ~DOF();

   void Init(const char* tableFilename, const char* romName);
   void DataReceive(char type, int number, int value);
   void Finish();

private:
   Pinball* m_pPinball;
};

} // namespace DOF
