#include "SequentialOutputSettings.h"

#include <algorithm>

namespace DOF
{

SequentialOutputSettings& SequentialOutputSettings::GetInstance()
{
   static SequentialOutputSettings instance;
   return instance;
}

SequentialOutputSettings::~SequentialOutputSettings() { Clear(); }

void SequentialOutputSettings::Clear()
{
   for (SequentialOutputSetting* setting : *this)
      delete setting;
   clear();
}

SequentialOutputDevice* SequentialOutputSettings::GetActiveSequentialDevice(const std::string& configPostfixID, int outputNumber) const
{
   for (SequentialOutputSetting* setting : *this)
   {
      for (SequentialOutputDevice* device : setting->GetDevices())
      {
         if (device->GetConfigPostfixID() == configPostfixID)
         {
            const std::vector<int>& outputNumbers = device->GetOutputNumbers();
            if (std::find(outputNumbers.begin(), outputNumbers.end(), outputNumber) != outputNumbers.end())
            {
               return device;
            }
         }
      }
   }
   return nullptr;
}

int SequentialOutputSettings::GetNextOutput(const std::string& configPostfixID, int outputNumber, int currentOutputNumber) const
{
   SequentialOutputDevice* device = GetActiveSequentialDevice(configPostfixID, outputNumber);
   if (device)
      return device->GetNextOutput(currentOutputNumber);

   return currentOutputNumber;
}

tinyxml2::XMLElement* SequentialOutputSettings::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   for (const SequentialOutputSetting* setting : *this)
   {
      tinyxml2::XMLElement* settingElement = setting->ToXml(doc);
      if (settingElement)
         element->InsertEndChild(settingElement);
   }

   return element;
}

bool SequentialOutputSettings::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   Clear();

   const tinyxml2::XMLElement* settingElement = element->FirstChildElement("SequentialOutputSetting");
   while (settingElement)
   {
      SequentialOutputSetting* setting = new SequentialOutputSetting();
      if (setting->FromXml(settingElement))
      {
         push_back(setting);
      }
      else
      {
         delete setting;
      }

      settingElement = settingElement->NextSiblingElement("SequentialOutputSetting");
   }

   return true;
}

}