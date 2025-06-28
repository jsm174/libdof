#pragma once

#include "../../general/generic/IXmlSerializable.h"
#include <string>
#include <vector>

namespace DOF
{

class ScheduledSettingDevice : public IXmlSerializable
{
public:
   ScheduledSettingDevice();
   virtual ~ScheduledSettingDevice() = default;

   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& value) { m_name = value; }

   int GetConfigPostfixID() const { return m_configPostfixID; }
   void SetConfigPostfixID(int value) { m_configPostfixID = value; }

   const std::string& GetOutputs() const { return m_outputs; }
   void SetOutputs(const std::string& value);

   const std::vector<int>& GetOutputList() const { return m_outputList; }

   int GetOutputPercent() const { return m_outputPercent; }
   void SetOutputPercent(int value);

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "ScheduledSettingDevice"; }

private:
   std::string m_name;
   int m_configPostfixID;
   std::string m_outputs;
   std::vector<int> m_outputList;
   int m_outputPercent;

   void ParseOutputs();
};

class ScheduledSettingDeviceList : public std::vector<ScheduledSettingDevice*>, public IXmlSerializable
{
public:
   ScheduledSettingDeviceList();
   virtual ~ScheduledSettingDeviceList();

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "ScheduledSettingDeviceList"; }

private:
   void Clear();
};

}