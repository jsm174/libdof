#pragma once

#include "DOF/DOF.h"

namespace DOF
{

class Log
{
 public:
  static const std::string& GetFilename();
  static void SetFilename(const std::string& szFilename) { m_szFilename = szFilename; }

  static void Init();
  static void AfterInit();
  static void Finish();
  static void WriteRaw(const char* format, ...);
  static void Write(const char* format, ...);
  static void Warning(const char* format, ...);
  static void Exception(const char* format, ...);
  static void Debug(const char* format, ...);

 private:
  Log();
  ~Log(){};

  static std::string m_szFilename;
};

}  // namespace DOF
