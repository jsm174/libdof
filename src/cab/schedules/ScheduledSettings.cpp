#include "ScheduledSettings.h"
#include "../../general/MathExtensions.h"

namespace DOF
{

ScheduledSettings& ScheduledSettings::GetInstance()
{
   static ScheduledSettings instance;
   return instance;
}

ScheduledSettings::~ScheduledSettings() { Clear(); }

void ScheduledSettings::Clear()
{
   for (ScheduledSetting* setting : *this)
      delete setting;
   clear();
}

ScheduledSetting* ScheduledSettings::GetActiveSchedule(const std::string& configPostfixID, int outputNumber) const
{
   return GetActiveSchedule(configPostfixID, outputNumber, std::chrono::system_clock::now());
}

ScheduledSetting* ScheduledSettings::GetActiveSchedule(const std::string& configPostfixID, int outputNumber, const std::chrono::system_clock::time_point& now) const
{
   for (ScheduledSetting* setting : *this)
   {
      if (setting->GetConfigPostfixID() == configPostfixID && setting->GetOutputNumber() == outputNumber && setting->IsTimeframeActive(now))
      {
         return setting;
      }
   }
   return nullptr;
}

uint8_t ScheduledSettings::GetNewRecalculatedOutput(const std::string& configPostfixID, int outputNumber, uint8_t originalOutput) const
{
   return GetNewRecalculatedOutput(configPostfixID, outputNumber, originalOutput, std::chrono::system_clock::now());
}

uint8_t ScheduledSettings::GetNewRecalculatedOutput(const std::string& configPostfixID, int outputNumber, uint8_t originalOutput, const std::chrono::system_clock::time_point& now) const
{
   ScheduledSetting* activeSetting = GetActiveSchedule(configPostfixID, outputNumber, now);
   if (!activeSetting)
      return originalOutput;


   int strength = activeSetting->GetOutputStrength();
   if (strength == 100)
      return originalOutput;


   int newValue = (originalOutput * strength) / 100;
   return static_cast<uint8_t>(MathExtensions::Limit(newValue, 0, 255));
}

XMLElement* ScheduledSettings::ToXml(XMLDocument& doc) const
{
   XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   for (const ScheduledSetting* setting : *this)
   {
      XMLElement* settingElement = setting->ToXml(doc);
      if (settingElement)
         element->InsertEndChild(settingElement);
   }

   return element;
}

bool ScheduledSettings::FromXml(const XMLElement* element)
{
   if (!element)
      return false;

   Clear();

   const XMLElement* settingElement = element->FirstChildElement("ScheduledSetting");
   while (settingElement)
   {
      ScheduledSetting* setting = new ScheduledSetting();
      if (setting->FromXml(settingElement))
      {
         push_back(setting);
      }
      else
      {
         delete setting;
      }

      settingElement = settingElement->NextSiblingElement("ScheduledSetting");
   }

   return true;
}

}