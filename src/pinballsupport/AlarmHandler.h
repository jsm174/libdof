#pragma once

#include "DOF/DOF.h"
#include "Action.h"
#include <functional>
#include <vector>
#include <mutex>
#include <chrono>
#include <memory>

namespace DOF
{

class Cabinet;
class Pinball;

class AlarmHandler
{
public:
   using AlarmCallback = Action;
   using TimePoint = std::chrono::steady_clock::time_point;

   AlarmHandler();
   ~AlarmHandler();

   void Init(Pinball* pPinball);
   void Finish();

   void RegisterAlarm(int durationMs, AlarmCallback alarmHandler, bool dontUnregister = false);
   void UnregisterAlarm(AlarmCallback alarmHandler);
   void RegisterAlarm(int durationMs, AlarmCallback alarmHandler, void* parameter, bool dontUnregister = false);
   void UnregisterAlarm(AlarmCallback alarmHandler, void* parameter);

   void RegisterIntervalAlarm(int intervalMs, AlarmCallback intervalAlarmHandler);
   void UnregisterIntervalAlarm(AlarmCallback intervalAlarmHandler);
   TimePoint GetNextAlarmTime();
   bool ExecuteAlarms(TimePoint alarmTime);

private:
   struct AlarmSetting
   {
      TimePoint alarmTime;
      AlarmCallback alarmHandler;
      void* parameter;
      bool hasParameter;
      bool dontUnregister;

      AlarmSetting(TimePoint time, AlarmCallback handler, bool dontUnreg = false)
         : alarmTime(time)
         , alarmHandler(handler)
         , parameter(nullptr)
         , hasParameter(false)
         , dontUnregister(dontUnreg)
      {
      }

      AlarmSetting(TimePoint time, AlarmCallback handler, void* param, bool dontUnreg = false)
         : alarmTime(time)
         , alarmHandler(handler)
         , parameter(param)
         , hasParameter(true)
         , dontUnregister(dontUnreg)
      {
      }

      void Execute() const
      {
         if (hasParameter)
            alarmHandler(parameter);
         else
            alarmHandler();
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