#pragma once

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

   const std::string& GetConfigPostfixID() const { return m_configPostfixID; }
   void SetConfigPostfixID(const std::string& value) { m_configPostfixID = value; }

   int GetOutputNumber() const { return m_outputNumber; }
   void SetOutputNumber(int value) { m_outputNumber = value; }

   const std::string& GetTimeframe() const { return m_timeframe; }
   void SetTimeframe(const std::string& value) { m_timeframe = value; }

   int GetOutputStrength() const { return m_outputStrength; }
   void SetOutputStrength(int value);

   bool IsTimeframeActive() const;
   bool IsTimeframeActive(const std::chrono::system_clock::time_point& now) const;


   virtual XMLElement* ToXml(XMLDocument& doc) const override;
   virtual bool FromXml(const XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "ScheduledSetting"; }

private:
   std::string m_configPostfixID;
   int m_outputNumber;
   std::string m_timeframe;
   int m_outputStrength;

   struct TimeRange
   {
      int startHour;
      int startMinute;
      int endHour;
      int endMinute;
      bool crossesMidnight;
   };

   TimeRange ParseTimeframe(const std::string& timeframe) const;
   bool IsTimeInRange(const TimeRange& range, int hour, int minute) const;
};

}