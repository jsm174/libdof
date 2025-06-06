#pragma once

#include "DOF/DOF.h"

namespace DOF
{

class Cabinet;
class Table;
class AlarmHandler;
class GlobalConfig;

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
   void Setup(const std::string& globalConfigFileName, const std::string& tableFilename, const std::string& romName);

   void Init();
   void ReceiveData(char type, int number, int value);
   void Finish();

private:
   Pinball();
   ~Pinball() { };

   void SetAlarms(AlarmHandler* pAlarms) { m_pAlarms = pAlarms; }
   void SetGlobalConfig(GlobalConfig* pGlobalConfig) { m_pGlobalConfig = pGlobalConfig; }

   static Pinball* m_pInstance;

   Cabinet* m_pCabinet;
   Table* m_pTable;
   AlarmHandler* m_pAlarms;
   GlobalConfig* m_pGlobalConfig;
};

} // namespace DOF
