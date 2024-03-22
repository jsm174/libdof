#pragma once

#ifdef _MSC_VER
#define LIBDOFAPI __declspec(dllexport)
#define LIBDOFCALLBACK __stdcall
#else
#define LIBDOFAPI __attribute__((visibility("default")))
#define LIBDOFCALLBACK
#endif

#include <cstdarg>
#include <string>

typedef void(LIBDOFCALLBACK* DOF_LogCallback)(const char* format, va_list args);

namespace DOF
{

class LIBDOFAPI Config
{
 public:
  static Config* GetInstance();

  void SetDOFServer(bool dofServer) { m_dofServer = dofServer; }
  bool IsDOFServer() { return m_dofServer; }
  void SetDOFServerAddr(const char* addr) { m_dofServerAddr = addr; }
  const char* GetDOFServerAddr() const { return m_dofServerAddr.c_str(); }
  void SetDOFServerPort(int port) { m_dofServerPort = port; }
  int GetDOFServerPort() const { return m_dofServerPort; }
  DOF_LogCallback GetLogCallback() const { return m_logCallback; }
  void SetLogCallback(DOF_LogCallback callback) { m_logCallback = callback; }

 private:
  Config();
  ~Config() {}

  static Config* m_pInstance;

  bool m_dofServer;
  std::string m_dofServerAddr;
  int m_dofServerPort;
  DOF_LogCallback m_logCallback;
};

}  // namespace DOF
