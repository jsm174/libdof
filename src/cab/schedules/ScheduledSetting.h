#pragma once

#include "ScheduledSettingDevice.h"
#include "../../general/generic/IXmlSerializable.h"
#include <string>
#include <chrono>

namespace DOF
{

class ScheduledSetting : public IXmlSerializable
{
public:
   ScheduledSetting();
   virtual ~ScheduledSetting() = default;

   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& value) { m_name = value; }

   bool IsEnabled() const { return m_enabled; }
   void SetEnabled(bool value) { m_enabled = value; }

   const std::string& GetClockStart() const { return m_clockStart; }
   void SetClockStart(const std::string& value) { m_clockStart = value; }

   const std::string& GetClockEnd() const { return m_clockEnd; }
   void SetClockEnd(const std::string& value) { m_clockEnd = value; }

   ScheduledSettingDeviceList& GetScheduledSettingDeviceList() { return m_scheduledSettingDeviceList; }
   const ScheduledSettingDeviceList& GetScheduledSettingDeviceList() const { return m_scheduledSettingDeviceList; }

   bool IsTimeInRange() const;
   bool IsTimeInRange(const std::chrono::system_clock::time_point& now) const;

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "ScheduledSetting"; }

private:
   std::string m_name;
   bool m_enabled;
   std::string m_clockStart;
   std::string m_clockEnd;
   ScheduledSettingDeviceList m_scheduledSettingDeviceList;

   struct TimeRange
   {
      int startHour;
      int startMinute;
      int endHour;
      int endMinute;
      bool crossesMidnight;
   };

   TimeRange ParseMilitaryTime(const std::string& clockStart, const std::string& clockEnd) const;
   bool IsTimeInRange(const TimeRange& range, int hour, int minute) const;
};

}