#include "SequentialOutputSetting.h"
#include "../../general/MathExtensions.h"

#include <sstream>
#include <algorithm>

namespace DOF
{

SequentialOutputDevice::SequentialOutputDevice()
   : m_outputMaxTime(500)
   , m_outputIndex(0)
   , m_outputTimestamp(std::chrono::steady_clock::now())
{
}

void SequentialOutputDevice::SetOutputIndex(int value)
{
   if (!m_outputNumbers.empty())
      m_outputIndex = MathExtensions::Limit(value, 0, static_cast<int>(m_outputNumbers.size() - 1));
   else
      m_outputIndex = 0;
}

int SequentialOutputDevice::GetNextOutput(int currentOutputNumber)
{
   if (m_outputNumbers.empty())
      return currentOutputNumber;

   auto now = std::chrono::steady_clock::now();
   auto timeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_outputTimestamp).count();

   if (timeDelta <= m_outputMaxTime)
   {
      m_outputIndex = (m_outputIndex + 1) % m_outputNumbers.size();
      m_outputTimestamp = now;
      return m_outputNumbers[m_outputIndex];
   }
   else
   {

      m_outputIndex = 0;
      m_outputTimestamp = now;
      return m_outputNumbers[m_outputIndex];
   }
}

tinyxml2::XMLElement* SequentialOutputDevice::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   if (!m_configPostfixID.empty())
      element->SetAttribute("ConfigPostfixID", m_configPostfixID.c_str());

   element->SetAttribute("OutputMaxTime", m_outputMaxTime);

   if (!m_outputNumbers.empty())
   {
      std::ostringstream oss;
      for (size_t i = 0; i < m_outputNumbers.size(); ++i)
      {
         if (i > 0)
            oss << ",";
         oss << m_outputNumbers[i];
      }
      element->SetAttribute("OutputNumbers", oss.str().c_str());
   }

   return element;
}

bool SequentialOutputDevice::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   const char* configPostfixID = element->Attribute("ConfigPostfixID");
   if (configPostfixID)
      m_configPostfixID = configPostfixID;

   element->QueryIntAttribute("OutputMaxTime", &m_outputMaxTime);

   const char* outputNumbers = element->Attribute("OutputNumbers");
   if (outputNumbers)
   {
      m_outputNumbers.clear();
      std::string str(outputNumbers);
      std::istringstream iss(str);
      std::string token;

      while (std::getline(iss, token, ','))
      {
         try
         {
            int number = std::stoi(token);
            m_outputNumbers.push_back(number);
         }
         catch (...)
         {
         }
      }

      SetOutputIndex(0);
   }

   return true;
}

SequentialOutputSetting::SequentialOutputSetting() { }

SequentialOutputSetting::~SequentialOutputSetting() { ClearDevices(); }

void SequentialOutputSetting::AddDevice(SequentialOutputDevice* device)
{
   if (device && std::find(m_devices.begin(), m_devices.end(), device) == m_devices.end())
      m_devices.push_back(device);
}

void SequentialOutputSetting::RemoveDevice(SequentialOutputDevice* device)
{
   auto it = std::find(m_devices.begin(), m_devices.end(), device);
   if (it != m_devices.end())
   {
      delete *it;
      m_devices.erase(it);
   }
}

void SequentialOutputSetting::ClearDevices()
{
   for (SequentialOutputDevice* device : m_devices)
      delete device;
   m_devices.clear();
}

tinyxml2::XMLElement* SequentialOutputSetting::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   for (const SequentialOutputDevice* device : m_devices)
   {
      tinyxml2::XMLElement* deviceElement = device->ToXml(doc);
      if (deviceElement)
         element->InsertEndChild(deviceElement);
   }

   return element;
}

bool SequentialOutputSetting::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   ClearDevices();

   const tinyxml2::XMLElement* deviceElement = element->FirstChildElement("SequentialOutputDevice");
   while (deviceElement)
   {
      SequentialOutputDevice* device = new SequentialOutputDevice();
      if (device->FromXml(deviceElement))
      {
         m_devices.push_back(device);
      }
      else
      {
         delete device;
      }

      deviceElement = deviceElement->NextSiblingElement("SequentialOutputDevice");
   }

   return true;
}

}