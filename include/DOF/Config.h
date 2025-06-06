#pragma once

#ifdef _MSC_VER
#define LIBDOFAPI __declspec(dllexport)
#define LIBDOFCALLBACK __stdcall
#else
#define LIBDOFAPI __attribute__((visibility("default")))
#define LIBDOFCALLBACK
#endif

#include <cstdarg>
#include <cstdint>
#include <string>

typedef enum
{
   DOF_LogLevel_INFO,
   DOF_LogLevel_WARN,
   DOF_LogLevel_ERROR,
   DOF_LogLevel_DEBUG
} DOF_LogLevel;

typedef void(LIBDOFCALLBACK* DOF_LogCallback)(DOF_LogLevel logLevel, const char* format, va_list args);

namespace DOF
{

class LIBDOFAPI Config
{
public:
   static Config* GetInstance();

   void SetBasePath(const char* basePath) { m_basePath = basePath; }
   const char* GetBasePath() const { return m_basePath.c_str(); }
   void SetDOFServer(bool dofServer) { m_dofServer = dofServer; }
   bool IsDOFServer() { return m_dofServer; }
   void SetDOFServerAddr(const char* addr) { m_dofServerAddr = addr; }
   const char* GetDOFServerAddr() const { return m_dofServerAddr.c_str(); }
   void SetDOFServerPort(int port) { m_dofServerPort = port; }
   int GetDOFServerPort() const { return m_dofServerPort; }
   DOF_LogLevel GetLogLevel() const { return m_logLevel; }
   void SetLogLevel(DOF_LogLevel logLevel) { m_logLevel = logLevel; }
   DOF_LogCallback GetLogCallback() const { return m_logCallback; }
   void SetLogCallback(DOF_LogCallback callback) { m_logCallback = callback; }

private:
   Config();
   ~Config() { }

   static Config* m_pInstance;

   std::string m_basePath;
   bool m_dofServer;
   std::string m_dofServerAddr;
   int m_dofServerPort;
   DOF_LogLevel m_logLevel;
   DOF_LogCallback m_logCallback;
};

} // namespace DOF
