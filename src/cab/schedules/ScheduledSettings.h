#pragma once

#include "ScheduledSetting.h"
#include "../../general/generic/IXmlSerializable.h"
#include <vector>
#include <memory>

namespace DOF
{

class IOutput;

class ScheduledSettings : public std::vector<ScheduledSetting*>, public IXmlSerializable
{
public:
   static ScheduledSettings& GetInstance();

   virtual ~ScheduledSettings();

   ScheduledSetting* GetActiveSchedule(const std::string& configPostfixID, int outputNumber) const;
   ScheduledSetting* GetActiveSchedule(const std::string& configPostfixID, int outputNumber, const std::chrono::system_clock::time_point& now) const;

   uint8_t GetNewRecalculatedOutput(const std::string& configPostfixID, int outputNumber, uint8_t originalOutput) const;
   uint8_t GetNewRecalculatedOutput(const std::string& configPostfixID, int outputNumber, uint8_t originalOutput, const std::chrono::system_clock::time_point& now) const;


   virtual XMLElement* ToXml(XMLDocument& doc) const override;
   virtual bool FromXml(const XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "ScheduledSettings"; }

private:
   ScheduledSettings() = default;
   ScheduledSettings(const ScheduledSettings&) = delete;
   ScheduledSettings& operator=(const ScheduledSettings&) = delete;

   void Clear();
};

}