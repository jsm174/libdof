#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "DOF/DOF.h"

namespace DOF
{

class Cabinet;
class Table;
class AlarmHandler;
class GlobalConfig;
class InputQueue;
class TableElementData;

class Pinball
{
public:
   static Pinball* GetInstance();

   void SetTable(Table* pTable) { m_pTable = pTable; }
   Table* GetTable() { return m_pTable; }
   void SetCabinet(Cabinet* pCabinet) { m_pCabinet = pCabinet; }
   Cabinet* GetCabinet() { return m_pCabinet; }
   AlarmHandler* GetAlarms() { return m_pAlarms; }
   GlobalConfig* GetGlobalConfig() { return m_pGlobalConfig; }
   const GlobalConfig* GetGlobalConfig() const { return m_pGlobalConfig; }
   void Setup(const std::string& globalConfigFileName = "", const std::string& tableFilename = "", const std::string& romName = "");
   void Init();
   void Finish();
   void MainThreadSignal();
   bool IsMainThreadActive() const { return m_mainThread.joinable(); }
   void ReceiveData(char type, int number, int value);
   void ReceiveData(const std::string& tableElementName, int value);
   void ReceiveData(const TableElementData& tableElementData);

private:
   Pinball();
   ~Pinball() { };

   void SetAlarms(AlarmHandler* pAlarms) { m_pAlarms = pAlarms; }
   void SetGlobalConfig(GlobalConfig* pGlobalConfig) { m_pGlobalConfig = pGlobalConfig; }

   void InitMainThread();
   void FinishMainThread();
   void MainThreadDoIt();

   static Pinball* m_pInstance;

   Cabinet* m_pCabinet;
   Table* m_pTable;
   AlarmHandler* m_pAlarms;
   GlobalConfig* m_pGlobalConfig;
   InputQueue* m_pInputQueue;

   std::thread m_mainThread;
   std::mutex m_mainThreadMutex;
   std::condition_variable m_mainThreadCV;
   std::atomic<bool> m_keepMainThreadAlive;
   std::atomic<bool> m_mainThreadDoWork;

   static const int MAX_INPUT_DATA_PROCESSING_TIME_MS = 10;
};

}
