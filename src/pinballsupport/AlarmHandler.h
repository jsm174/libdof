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
   void RegisterIntervalAlarm(int intervalMs, AlarmCallback intervalAlarmHandler);
   void UnregisterAlarm(AlarmCallback alarmHandler);
   void UnregisterIntervalAlarm(AlarmCallback intervalAlarmHandler);
   TimePoint GetNextAlarmTime();
   bool ExecuteAlarms(TimePoint alarmTime);

private:
   struct AlarmSetting
   {
      TimePoint alarmTime;
      AlarmCallback alarmHandler;
      bool dontUnregister;

      AlarmSetting(TimePoint time, AlarmCallback handler, bool dontUnreg = false)
         : alarmTime(time)
         , alarmHandler(handler)
         , dontUnregister(dontUnreg)
      {
      }
   };

   struct IntervalAlarmSetting
   {
      int intervalMs;
      TimePoint nextAlarm;
      AlarmCallback intervalAlarmHandler;

      IntervalAlarmSetting(int interval, AlarmCallback handler)
         : intervalMs(interval)
         , intervalAlarmHandler(handler)
         , nextAlarm(std::chrono::steady_clock::now() + std::chrono::milliseconds(interval))
      {
      }
   };

   std::recursive_mutex m_alarmMutex;
   std::recursive_mutex m_intervalAlarmMutex;
   std::vector<AlarmSetting> m_alarmList;
   std::vector<IntervalAlarmSetting> m_intervalAlarmList;

   TimePoint GetNextAlarm();
   TimePoint GetNextIntervalAlarm();
   bool ProcessAlarms(TimePoint alarmTime);
   bool ProcessIntervalAlarms(TimePoint alarmTime);
};

}