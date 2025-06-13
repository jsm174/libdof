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
   m_alarmList.clear();
}

void AlarmHandler::RegisterAlarm(int durationMs, AlarmCallback alarmHandler, bool dontUnregister)
{
   std::lock_guard<std::recursive_mutex> lock(m_alarmMutex);

   Log::Debug(StringExtensions::Build("AlarmHandler::RegisterAlarm: duration={0}ms, dontUnregister={1}, currentAlarms={2}", std::to_string(durationMs), dontUnregister ? "true" : "false",
      std::to_string(static_cast<int>(m_alarmList.size()))));

   TimePoint alarmTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(durationMs);
   m_alarmList.emplace_back(alarmTime, alarmHandler, nullptr);

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

void AlarmHandler::RegisterAlarmForEffect(int durationMs, AlarmCallback alarmHandler, void* effectPtr)
{
   std::lock_guard<std::recursive_mutex> lock(m_alarmMutex);

   Log::Debug(StringExtensions::Build("AlarmHandler::RegisterAlarmForEffect: duration={0}ms, effect={1}, currentAlarms={2}", std::to_string(durationMs),
      StringExtensions::ToAddressString(effectPtr), std::to_string(static_cast<int>(m_alarmList.size()))));

   TimePoint alarmTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(durationMs);
   m_alarmList.emplace_back(alarmTime, alarmHandler, effectPtr);

   Log::Debug(StringExtensions::Build("AlarmHandler::RegisterAlarmForEffect: alarm registered for effect {0}, totalAlarms={1}", StringExtensions::ToAddressString(effectPtr),
      std::to_string(static_cast<int>(m_alarmList.size()))));
}

void AlarmHandler::UnregisterAlarmsForEffect(void* effectPtr)
{
   std::lock_guard<std::recursive_mutex> lock(m_alarmMutex);

   auto it = m_alarmList.begin();
   while (it != m_alarmList.end())
   {
      if (it->effectPtr == effectPtr)
      {
         Log::Debug(StringExtensions::Build("AlarmHandler::UnregisterAlarmsForEffect: removing alarm for effect {0}", StringExtensions::ToAddressString(effectPtr)));
         it = m_alarmList.erase(it);
      }
      else
      {
         ++it;
      }
   }
}

AlarmHandler::TimePoint AlarmHandler::GetNextAlarmTime() { return GetNextAlarm(); }

bool AlarmHandler::ExecuteAlarms(TimePoint alarmTime)
{
   bool result = ProcessAlarms(alarmTime);
   if (result)
   {
      Log::Debug("AlarmHandler::ExecuteAlarms: Processed alarms");
   }
   return result;
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

}