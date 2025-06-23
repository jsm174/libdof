#include "ScheduledSettings.h"
#include "ScheduledSettingDevice.h"
#include "../../general/MathExtensions.h"
#include "../out/IOutput.h"
#include "../out/Output.h"
#include "../../Log.h"
#include <algorithm>

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

ScheduledSetting* ScheduledSettings::GetActiveSchedule(const std::string& configPostfixID, int outputNumber, const std::chrono::system_clock::time_point& now) const { return nullptr; }

uint8_t ScheduledSettings::GetNewRecalculatedOutput(const std::string& configPostfixID, int outputNumber, uint8_t originalOutput) const
{
   return GetNewRecalculatedOutput(configPostfixID, outputNumber, originalOutput, std::chrono::system_clock::now());
}

uint8_t ScheduledSettings::GetNewRecalculatedOutput(const std::string& configPostfixID, int outputNumber, uint8_t originalOutput, const std::chrono::system_clock::time_point& now) const
{
   return originalOutput;
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
   if (!currentOutput)
      return nullptr;

   std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
   int deviceID = startingDeviceIndex + currentDeviceIndex;

   static std::vector<int> cacheList;

   static int callCount = 0;
   if (++callCount > 1000)
   {
      cacheList.clear();
      callCount = 0;
   }

   if (std::find(cacheList.begin(), cacheList.end(), deviceID) != cacheList.end())
      return nullptr;

   for (ScheduledSetting* scheduledSetting : *this)
   {
      if (!scheduledSetting || !scheduledSetting->IsEnabled())
         continue;

      if (!scheduledSetting->IsTimeInRange(now))
         continue;

      const ScheduledSettingDeviceList& deviceList = scheduledSetting->GetScheduledSettingDeviceList();
      for (ScheduledSettingDevice* device : deviceList)
      {
         if (!device)
            continue;

         if (device->GetConfigPostfixID() != deviceID)
            continue;

         const std::vector<int>& outputList = device->GetOutputList();
         int outputNumber = currentOutput->GetNumber();

         if (!outputList.empty())
         {
            if (std::find(outputList.begin(), outputList.end(), outputNumber) == outputList.end())
               continue;
         }

         if (recalculateOutputValue)
         {
            int outputPercent = device->GetOutputPercent();
            if (outputPercent != 100)
            {
               int originalValue = currentOutput->GetOutput();
               int newValue = (originalValue * outputPercent) / 100;
               newValue = MathExtensions::Limit(newValue, 0, 255);

               currentOutput->SetOutput(static_cast<uint8_t>(newValue));

               Log::Debug("ScheduledSettings: Applied " + std::to_string(outputPercent) + "% strength to output " + std::to_string(outputNumber) + " on device " + std::to_string(deviceID)
                  + " (value: " + std::to_string(originalValue) + " -> " + std::to_string(newValue) + ")");
            }
         }

         cacheList.push_back(deviceID);

         return device;
      }
   }

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