#pragma once

#include "../../general/generic/IXmlSerializable.h"
#include <string>
#include <vector>
#include <chrono>

namespace DOF
{

class SequentialOutputDevice : public IXmlSerializable
{
public:
   SequentialOutputDevice();
   virtual ~SequentialOutputDevice() = default;

   const std::string& GetConfigPostfixID() const { return m_configPostfixID; }
   void SetConfigPostfixID(const std::string& value) { m_configPostfixID = value; }

   const std::vector<int>& GetOutputNumbers() const { return m_outputNumbers; }
   void SetOutputNumbers(const std::vector<int>& value) { m_outputNumbers = value; }

   int GetOutputMaxTime() const { return m_outputMaxTime; }
   void SetOutputMaxTime(int value) { m_outputMaxTime = value; }

   int GetOutputIndex() const { return m_outputIndex; }
   void SetOutputIndex(int value);

   std::chrono::steady_clock::time_point GetOutputTimestamp() const { return m_outputTimestamp; }
   void SetOutputTimestamp(const std::chrono::steady_clock::time_point& value) { m_outputTimestamp = value; }

   int GetNextOutput(int currentOutputNumber);


   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "SequentialOutputDevice"; }

private:
   std::string m_configPostfixID;
   std::vector<int> m_outputNumbers;
   int m_outputMaxTime;
   int m_outputIndex;
   std::chrono::steady_clock::time_point m_outputTimestamp;
};

class SequentialOutputSetting : public IXmlSerializable
{
public:
   SequentialOutputSetting();
   virtual ~SequentialOutputSetting();

   const std::vector<SequentialOutputDevice*>& GetDevices() const { return m_devices; }
   void AddDevice(SequentialOutputDevice* device);
   void RemoveDevice(SequentialOutputDevice* device);
   void ClearDevices();


   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "SequentialOutputSetting"; }

private:
   std::vector<SequentialOutputDevice*> m_devices;
};

}