#include "ScheduledSetting.h"
#include "../../general/MathExtensions.h"
#include <tinyxml2/tinyxml2.h>

#include <sstream>
#include <iomanip>

namespace DOF
{

ScheduledSetting::ScheduledSetting()
   : m_name("")
   , m_enabled(true)
   , m_clockStart("")
   , m_clockEnd("")
{
}

bool ScheduledSetting::IsTimeInRange() const { return IsTimeInRange(std::chrono::system_clock::now()); }

bool ScheduledSetting::IsTimeInRange(const std::chrono::system_clock::time_point& now) const
{
   if (m_clockStart.empty() || m_clockEnd.empty())
      return false;

   TimeRange range = ParseMilitaryTime(m_clockStart, m_clockEnd);

   std::time_t time = std::chrono::system_clock::to_time_t(now);
   std::tm* localTime = std::localtime(&time);

   return IsTimeInRange(range, localTime->tm_hour, localTime->tm_min);
}

ScheduledSetting::TimeRange ScheduledSetting::ParseMilitaryTime(const std::string& clockStart, const std::string& clockEnd) const
{
   TimeRange range = { 0, 0, 0, 0, false };

   try
   {
      if (clockStart.length() >= 4)
      {
         range.startHour = std::stoi(clockStart.substr(0, 2));
         range.startMinute = std::stoi(clockStart.substr(2, 2));
      }

      if (clockEnd.length() >= 4)
      {
         range.endHour = std::stoi(clockEnd.substr(0, 2));
         range.endMinute = std::stoi(clockEnd.substr(2, 2));
      }

      int startMinutes = range.startHour * 60 + range.startMinute;
      int endMinutes = range.endHour * 60 + range.endMinute;
      range.crossesMidnight = (startMinutes >= endMinutes);
   }
   catch (...)
   {
   }

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

   if (!m_name.empty())
   {
      tinyxml2::XMLElement* nameElement = doc.NewElement("Name");
      nameElement->SetText(m_name.c_str());
      element->InsertEndChild(nameElement);
   }

   tinyxml2::XMLElement* enabledElement = doc.NewElement("Enabled");
   enabledElement->SetText(m_enabled ? "true" : "false");
   element->InsertEndChild(enabledElement);

   if (!m_clockStart.empty())
   {
      tinyxml2::XMLElement* startElement = doc.NewElement("ClockStart");
      startElement->SetText(m_clockStart.c_str());
      element->InsertEndChild(startElement);
   }

   if (!m_clockEnd.empty())
   {
      tinyxml2::XMLElement* endElement = doc.NewElement("ClockEnd");
      endElement->SetText(m_clockEnd.c_str());
      element->InsertEndChild(endElement);
   }

   tinyxml2::XMLElement* devicesElement = m_scheduledSettingDeviceList.ToXml(doc);
   if (devicesElement)
      element->InsertEndChild(devicesElement);

   return element;
}

bool ScheduledSetting::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   const tinyxml2::XMLElement* nameElement = element->FirstChildElement("Name");
   if (nameElement && nameElement->GetText())
      m_name = nameElement->GetText();

   const tinyxml2::XMLElement* enabledElement = element->FirstChildElement("Enabled");
   if (enabledElement && enabledElement->GetText())
   {
      std::string enabledText = enabledElement->GetText();
      m_enabled = (enabledText == "true" || enabledText == "True");
   }

   const tinyxml2::XMLElement* startElement = element->FirstChildElement("ClockStart");
   if (startElement && startElement->GetText())
      m_clockStart = startElement->GetText();

   const tinyxml2::XMLElement* endElement = element->FirstChildElement("ClockEnd");
   if (endElement && endElement->GetText())
      m_clockEnd = endElement->GetText();

   const tinyxml2::XMLElement* devicesElement = element->FirstChildElement("ScheduledSettingDeviceList");
   if (devicesElement)
      m_scheduledSettingDeviceList.FromXml(devicesElement);

   return true;
}

}