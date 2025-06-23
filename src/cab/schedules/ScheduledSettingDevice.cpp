#include "ScheduledSettingDevice.h"
#include "../../general/StringExtensions.h"
#include "../../general/MathExtensions.h"
#include "../../Log.h"
#include <tinyxml2/tinyxml2.h>
#include <sstream>

namespace DOF
{

ScheduledSettingDevice::ScheduledSettingDevice()
   : m_name("")
   , m_configPostfixID(0)
   , m_outputs("")
   , m_outputPercent(100)
{
}

void ScheduledSettingDevice::SetOutputs(const std::string& value)
{
   m_outputs = value;
   ParseOutputs();
}

void ScheduledSettingDevice::SetOutputPercent(int value) { m_outputPercent = MathExtensions::Limit(value, 0, 100); }

void ScheduledSettingDevice::ParseOutputs()
{
   m_outputList.clear();

   if (m_outputs.empty())
      return;

   std::vector<std::string> parts = StringExtensions::Split(m_outputs, { ',' });

   for (const std::string& part : parts)
   {
      std::string trimmed = StringExtensions::Trim(part);
      if (!trimmed.empty())
      {
         try
         {
            int outputNum = std::stoi(trimmed);
            if (outputNum > 0)
               m_outputList.push_back(outputNum);
         }
         catch (...)
         {
         }
      }
   }
}

tinyxml2::XMLElement* ScheduledSettingDevice::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   if (!m_name.empty())
   {
      tinyxml2::XMLElement* nameElement = doc.NewElement("Name");
      nameElement->SetText(m_name.c_str());
      element->InsertEndChild(nameElement);
   }

   tinyxml2::XMLElement* configElement = doc.NewElement("ConfigPostfixID");
   configElement->SetText(m_configPostfixID);
   element->InsertEndChild(configElement);

   if (!m_outputs.empty())
   {
      tinyxml2::XMLElement* outputsElement = doc.NewElement("Outputs");
      outputsElement->SetText(m_outputs.c_str());
      element->InsertEndChild(outputsElement);
   }

   tinyxml2::XMLElement* percentElement = doc.NewElement("OutputPercent");
   percentElement->SetText(m_outputPercent);
   element->InsertEndChild(percentElement);

   return element;
}

bool ScheduledSettingDevice::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   const tinyxml2::XMLElement* nameElement = element->FirstChildElement("Name");
   if (nameElement && nameElement->GetText())
      m_name = nameElement->GetText();

   const tinyxml2::XMLElement* configElement = element->FirstChildElement("ConfigPostfixID");
   if (configElement)
      configElement->QueryIntText(&m_configPostfixID);

   const tinyxml2::XMLElement* outputsElement = element->FirstChildElement("Outputs");
   if (outputsElement && outputsElement->GetText())
      SetOutputs(outputsElement->GetText());

   const tinyxml2::XMLElement* percentElement = element->FirstChildElement("OutputPercent");
   if (percentElement)
   {
      int percent;
      if (percentElement->QueryIntText(&percent) == tinyxml2::XML_SUCCESS)
         SetOutputPercent(percent);
   }

   return true;
}

// ScheduledSettingDeviceList implementation

ScheduledSettingDeviceList::ScheduledSettingDeviceList() { }

ScheduledSettingDeviceList::~ScheduledSettingDeviceList() { Clear(); }

void ScheduledSettingDeviceList::Clear()
{
   for (ScheduledSettingDevice* device : *this)
      delete device;
   clear();
}

tinyxml2::XMLElement* ScheduledSettingDeviceList::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   for (const ScheduledSettingDevice* device : *this)
   {
      if (device)
      {
         tinyxml2::XMLElement* deviceElement = device->ToXml(doc);
         if (deviceElement)
            element->InsertEndChild(deviceElement);
      }
   }

   return element;
}

bool ScheduledSettingDeviceList::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   Clear();

   for (const tinyxml2::XMLElement* deviceElement = element->FirstChildElement("ScheduledSettingDevice"); deviceElement;
      deviceElement = deviceElement->NextSiblingElement("ScheduledSettingDevice"))
   {
      ScheduledSettingDevice* device = new ScheduledSettingDevice();
      if (device->FromXml(deviceElement))
         push_back(device);
      else
         delete device;
   }

   return true;
}

}