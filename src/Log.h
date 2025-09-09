#pragma once

#include "DOF/DOF.h"
#include <unordered_set>
#include <vector>
#include <fstream>
#include <mutex>

namespace DOF
{

class Log
{
public:
   static const std::string& GetFilename() { return m_filename; }
   static void SetFilename(const std::string& filename) { m_filename = filename; }

   static std::string GetInstrumentations() { return m_instrumentations; }
   static void SetInstrumentations(const std::string& instrumentations);

   static void Init(bool enableLogging = true);
   static void AfterInit();
   static void Finish();
   static void Write(const std::string& message);
   static void Error(const std::string& message);
   static void Warning(const std::string& message);
   static void Exception(const std::string& message);
   static void Debug(const std::string& message);
   static void Once(const std::string& key, const std::string& message);
   static void Instrumentation(const std::string& key, const std::string& message);

private:
   Log();
   ~Log() { };

   static void WriteRaw(const std::string& message);
   static void WriteToFile(const std::string& message);

   static std::ofstream m_logger;
   static bool m_isInitialized;
   static bool m_isOk;
   static bool m_isEnabled;
   static std::mutex m_locker;

   static std::string m_filename;
   static std::string m_instrumentations;
   static std::unordered_set<std::string> m_activeInstrumentations;
   static std::vector<std::string> m_preLogFileLog;
   static std::unordered_set<std::string> m_onceKeys;
};

}
