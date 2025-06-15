#pragma once

#include "DOF/DOF.h"
#include <functional>
#include <vector>
#include <mutex>
#include <chrono>

namespace DOF
{

class Cabinet;
class Pinball;

class AlarmHandler
{
public:
   using AlarmCallback = std::function<void()>;
   using TimePoint = std::chrono::steady_clock::time_point;

   AlarmHandler();
   ~AlarmHandler();

   void Init(Pinball* pPinball);
   void Finish();
   void RegisterAlarm(int durationMs, AlarmCallback alarmHandler, bool dontUnregister = false);
   void RegisterAlarmForEffect(int durationMs, AlarmCallback alarmHandler, void* effectPtr);
   void UnregisterAlarm(AlarmCallback alarmHandler);
   void UnregisterAlarmsForEffect(void* effectPtr);
   TimePoint GetNextAlarmTime();
   bool ExecuteAlarms(TimePoint alarmTime);

private:
   struct AlarmSetting
   {
      TimePoint alarmTime;
      AlarmCallback alarmHandler;
      void* effectPtr;

      AlarmSetting(TimePoint time, AlarmCallback handler, void* effect = nullptr)
         : alarmTime(time)
         , alarmHandler(handler)
         , effectPtr(effect)
      {
      }
   };

   std::recursive_mutex m_alarmMutex;
   std::vector<AlarmSetting> m_alarmList;

   TimePoint GetNextAlarm();
   bool ProcessAlarms(TimePoint alarmTime);
};

}