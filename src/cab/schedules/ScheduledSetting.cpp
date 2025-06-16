#include "ScheduledSetting.h"
#include "../../general/MathExtensions.h"

#include <sstream>
#include <iomanip>

namespace DOF
{

ScheduledSetting::ScheduledSetting()
   : m_outputNumber(0)
   , m_outputStrength(100)
{
}

void ScheduledSetting::SetOutputStrength(int value) { m_outputStrength = MathExtensions::Limit(value, 0, 100); }

bool ScheduledSetting::IsTimeframeActive() const { return IsTimeframeActive(std::chrono::system_clock::now()); }

bool ScheduledSetting::IsTimeframeActive(const std::chrono::system_clock::time_point& now) const
{
   if (m_timeframe.empty())
      return false;

   TimeRange range = ParseTimeframe(m_timeframe);

   std::time_t time = std::chrono::system_clock::to_time_t(now);
   std::tm* localTime = std::localtime(&time);

   return IsTimeInRange(range, localTime->tm_hour, localTime->tm_min);
}

ScheduledSetting::TimeRange ScheduledSetting::ParseTimeframe(const std::string& timeframe) const
{
   TimeRange range = { 0, 0, 0, 0, false };

   size_t dashPos = timeframe.find('-');
   if (dashPos == std::string::npos)
      return range;

   std::string startTime = timeframe.substr(0, dashPos);
   std::string endTime = timeframe.substr(dashPos + 1);

   if (startTime.length() >= 4)
   {
      range.startHour = std::stoi(startTime.substr(0, 2));
      range.startMinute = std::stoi(startTime.substr(2, 2));
   }

   if (endTime.length() >= 4)
   {
      range.endHour = std::stoi(endTime.substr(0, 2));
      range.endMinute = std::stoi(endTime.substr(2, 2));
   }

   int startMinutes = range.startHour * 60 + range.startMinute;
   int endMinutes = range.endHour * 60 + range.endMinute;
   range.crossesMidnight = (startMinutes >= endMinutes);

   return range;
}

bool ScheduledSetting::IsTimeInRange(const TimeRange& range, int hour, int minute) const
{
   int currentMinutes = hour * 60 + minute;
   int startMinutes = range.startHour * 60 + range.startMinute;
   int endMinutes = range.endHour * 60 + range.endMinute;

   if (range.crossesMidnight)
   {

      return (currentMinutes >= startMinutes) || (currentMinutes <= endMinutes);
   }
   else
   {

      return (currentMinutes >= startMinutes) && (currentMinutes <= endMinutes);
   }
}

tinyxml2::XMLElement* ScheduledSetting::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   if (!m_configPostfixID.empty())
      element->SetAttribute("ConfigPostfixID", m_configPostfixID.c_str());

   element->SetAttribute("OutputNumber", m_outputNumber);

   if (!m_timeframe.empty())
      element->SetAttribute("Timeframe", m_timeframe.c_str());

   element->SetAttribute("OutputStrength", m_outputStrength);

   return element;
}

bool ScheduledSetting::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   const char* configPostfixID = element->Attribute("ConfigPostfixID");
   if (configPostfixID)
      m_configPostfixID = configPostfixID;

   element->QueryIntAttribute("OutputNumber", &m_outputNumber);

   const char* timeframe = element->Attribute("Timeframe");
   if (timeframe)
      m_timeframe = timeframe;

   element->QueryIntAttribute("OutputStrength", &m_outputStrength);
   SetOutputStrength(m_outputStrength);

   return true;
}

}