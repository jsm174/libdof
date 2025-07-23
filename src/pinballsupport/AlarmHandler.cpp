#include "AlarmHandler.h"
#include "../Pinball.h"
#include "../general/StringExtensions.h"
#include "../Log.h"
#include <algorithm>
#include <chrono>

namespace DOF
{

AlarmHandler::AlarmHandler() { }

AlarmHandler::~AlarmHandler() { Finish(); }

void AlarmHandler::Init(Pinball* pPinball) { }

void AlarmHandler::Finish()
{
   std::lock_guard<std::recursive_mutex> alarmLock(m_alarmMutex);
   std::lock_guard<std::recursive_mutex> intervalLock(m_intervalAlarmMutex);
   m_alarmList.clear();
   m_intervalAlarmList.clear();
}

void AlarmHandler::RegisterAlarm(int durationMs, AlarmCallback alarmHandler, bool dontUnregister)
{
   std::lock_guard<std::recursive_mutex> lock(m_alarmMutex);

   Log::Debug(StringExtensions::Build("AlarmHandler::RegisterAlarm: duration={0}ms, dontUnregister={1}, currentAlarms={2}", std::to_string(durationMs), dontUnregister ? "true" : "false",
      std::to_string(static_cast<int>(m_alarmList.size()))));

   TimePoint alarmTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(durationMs);
   m_alarmList.emplace_back(alarmTime, alarmHandler, dontUnregister);

   Log::Debug(StringExtensions::Build("AlarmHandler::RegisterAlarm: alarm registered, totalAlarms={0}", std::to_string(static_cast<int>(m_alarmList.size()))));
}

void AlarmHandler::UnregisterAlarm(AlarmCallback alarmHandler)
{
   std::lock_guard<std::recursive_mutex> lock(m_alarmMutex);

   auto it = m_alarmList.begin();
   while (it != m_alarmList.end())
   {
      if (it->alarmHandler.target<void (*)()>() == alarmHandler.target<void (*)()>())
      {
         Log::Debug(StringExtensions::Build("AlarmHandler::UnregisterAlarm: removing alarm, remaining={0}", std::to_string(static_cast<int>(m_alarmList.size() - 1))));
         it = m_alarmList.erase(it);
      }
      else
      {
         ++it;
      }
   }
}

AlarmHandler::TimePoint AlarmHandler::GetNextAlarmTime()
{
   TimePoint nextAlarm = GetNextAlarm();
   TimePoint nextIntervalAlarm = GetNextIntervalAlarm();
   return (nextIntervalAlarm < nextAlarm) ? nextIntervalAlarm : nextAlarm;
}

bool AlarmHandler::ExecuteAlarms(TimePoint alarmTime)
{
   bool result = ProcessAlarms(alarmTime);
   bool intervalResult = ProcessIntervalAlarms(alarmTime);
   if (result || intervalResult)
   {
      Log::Debug("AlarmHandler::ExecuteAlarms: Processed alarms");
   }
   return result || intervalResult;
}

AlarmHandler::TimePoint AlarmHandler::GetNextAlarm()
{
   std::lock_guard<std::recursive_mutex> lock(m_alarmMutex);

   if (m_alarmList.empty())
   {
      return TimePoint::max();
   }

   auto minElement = std::min_element(m_alarmList.begin(), m_alarmList.end(), [](const AlarmSetting& a, const AlarmSetting& b) { return a.alarmTime < b.alarmTime; });

   return minElement->alarmTime;
}

bool AlarmHandler::ProcessAlarms(TimePoint alarmTime)
{
   std::vector<AlarmSetting> toExecute;

   {
      std::lock_guard<std::recursive_mutex> lock(m_alarmMutex);

      auto it = std::partition(m_alarmList.begin(), m_alarmList.end(), [alarmTime](const AlarmSetting& alarm) { return alarm.alarmTime > alarmTime; });

      std::move(it, m_alarmList.end(), std::back_inserter(toExecute));
      m_alarmList.erase(it, m_alarmList.end());
   }

   bool alarmsExecuted = !toExecute.empty();

   if (alarmsExecuted)
   {
      Log::Debug(StringExtensions::Build(
         "AlarmHandler::ProcessAlarms: executing {0} alarms, {1} remaining", std::to_string(static_cast<int>(toExecute.size())), std::to_string(static_cast<int>(m_alarmList.size()))));
   }

   for (const auto& alarm : toExecute)
   {
      try
      {
         Log::Debug("AlarmHandler::ProcessAlarms: calling alarm function");
         alarm.alarmHandler();
         Log::Debug("AlarmHandler::ProcessAlarms: alarm function completed");
      }
      catch (...)
      {
         Log::Debug("AlarmHandler::ProcessAlarms: alarm function threw exception");
      }
   }

   return alarmsExecuted;
}

void AlarmHandler::RegisterIntervalAlarm(int intervalMs, void* owner, AlarmCallback intervalAlarmHandler)
{
   std::lock_guard<std::recursive_mutex> lock(m_intervalAlarmMutex);

   UnregisterIntervalAlarm(owner);
   m_intervalAlarmList.emplace_back(intervalMs, intervalAlarmHandler, owner);

   Log::Debug(StringExtensions::Build("AlarmHandler::RegisterIntervalAlarm: interval={0}ms, owner={1}, totalIntervalAlarms={2}", std::to_string(intervalMs), std::to_string((uintptr_t)owner),
      std::to_string(static_cast<int>(m_intervalAlarmList.size()))));
}

void AlarmHandler::UnregisterIntervalAlarm(void* owner)
{
   std::lock_guard<std::recursive_mutex> lock(m_intervalAlarmMutex);

   auto it = m_intervalAlarmList.begin();
   while (it != m_intervalAlarmList.end())
   {
      if (it->owner == owner)
      {
         Log::Debug(StringExtensions::Build("AlarmHandler::UnregisterIntervalAlarm: removing alarm for owner={0}, remaining={1}", std::to_string((uintptr_t)owner),
            std::to_string(static_cast<int>(m_intervalAlarmList.size() - 1))));
         it = m_intervalAlarmList.erase(it);
      }
      else
      {
         ++it;
      }
   }
}

AlarmHandler::TimePoint AlarmHandler::GetNextIntervalAlarm()
{
   std::lock_guard<std::recursive_mutex> lock(m_intervalAlarmMutex);

   if (m_intervalAlarmList.empty())
   {
      return TimePoint::max();
   }

   auto minElement
      = std::min_element(m_intervalAlarmList.begin(), m_intervalAlarmList.end(), [](const IntervalAlarmSetting& a, const IntervalAlarmSetting& b) { return a.nextAlarm < b.nextAlarm; });

   return minElement->nextAlarm;
}

bool AlarmHandler::ProcessIntervalAlarms(TimePoint alarmTime)
{
   std::vector<AlarmCallback> toExecute;
   std::vector<size_t> toUpdateIndices;

   {
      std::lock_guard<std::recursive_mutex> lock(m_intervalAlarmMutex);

      for (size_t i = 0; i < m_intervalAlarmList.size(); ++i)
      {
         if (m_intervalAlarmList[i].nextAlarm <= alarmTime)
         {
            toExecute.push_back(m_intervalAlarmList[i].intervalAlarmHandler);
            toUpdateIndices.push_back(i);
         }
      }
   }

   bool alarmsExecuted = !toExecute.empty();

   if (alarmsExecuted)
   {
      Log::Debug(StringExtensions::Build("AlarmHandler::ProcessIntervalAlarms: executing {0} interval alarms", std::to_string(static_cast<int>(toExecute.size()))));
   }

   for (const auto& alarmHandler : toExecute)
   {
      try
      {
         Log::Debug("AlarmHandler::ProcessIntervalAlarms: calling interval alarm function");
         alarmHandler();
         Log::Debug("AlarmHandler::ProcessIntervalAlarms: interval alarm function completed");
      }
      catch (...)
      {
         Log::Debug("AlarmHandler::ProcessIntervalAlarms: interval alarm function threw exception");
      }
   }

   {
      std::lock_guard<std::recursive_mutex> lock(m_intervalAlarmMutex);

      for (size_t i : toUpdateIndices)
      {
         if (i < m_intervalAlarmList.size())
         {
            auto& intervalAlarm = m_intervalAlarmList[i];
            if (intervalAlarm.nextAlarm + std::chrono::milliseconds(intervalAlarm.intervalMs) <= alarmTime)
            {
               intervalAlarm.nextAlarm = alarmTime + std::chrono::milliseconds(1);
            }
            else
            {
               intervalAlarm.nextAlarm = intervalAlarm.nextAlarm + std::chrono::milliseconds(intervalAlarm.intervalMs);
            }
         }
      }
   }

   return alarmsExecuted;
}

}