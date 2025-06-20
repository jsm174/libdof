#include "ScheduledSettings.h"
#include "../../general/MathExtensions.h"
#include "../out/IOutput.h"
#include "../out/Output.h"

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

tinyxml2::XMLElement* ScheduledSettings::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   for (const ScheduledSetting* setting : *this)
   {
      tinyxml2::XMLElement* settingElement = setting->ToXml(doc);
      if (settingElement)
         element->InsertEndChild(settingElement);
   }

   return element;
}

bool ScheduledSettings::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   Clear();

   const tinyxml2::XMLElement* settingElement = element->FirstChildElement("ScheduledSetting");
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

ScheduledSettingDevice* ScheduledSettings::GetActiveSchedule(IOutput* currentOutput, bool recalculateOutputValue, int startingDeviceIndex, int currentDeviceIndex)
{
   // Stub implementation matching C# interface - actual scheduling logic would go here
   return nullptr;
}

IOutput* ScheduledSettings::GetNewRecalculatedOutput(IOutput* currentOutput, int startingDeviceIndex, int currentDeviceIndex)
{
   if (currentOutput->GetOutput() != 0)
   {
      Output* newOutput = new Output();
      newOutput->SetOutput(currentOutput->GetOutput());
      newOutput->SetNumber(currentOutput->GetNumber());

      ScheduledSettingDevice* activeScheduleDevice = GetActiveSchedule(newOutput, true, startingDeviceIndex, currentDeviceIndex);

      if (activeScheduleDevice)
         return newOutput;
      else
      {
         delete newOutput;
         return currentOutput;
      }
   }
   else
   {
      return currentOutput;
   }
}

}