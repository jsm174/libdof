#pragma once

#include "../OutputControllerCompleteBase.h"
#include "UMXDevice.h"
#include <string>
#include <chrono>
#include <mutex>

namespace DOF
{

class Cabinet;

class UMXController : public OutputControllerCompleteBase
{
public:
   UMXController();
   virtual ~UMXController();

   int GetNumber() const { return m_number; }
   void SetNumber(int value);

   void UpdateCabinetFromConfig(Cabinet* cabinet);

   virtual std::string GetXmlElementName() const override { return "UMXController"; }
   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;

protected:
   int GetNumberOfConfiguredOutputs() override;
   bool VerifySettings() override;
   void ConnectToController() override;
   void DisconnectFromController() override;
   void UpdateOutputs(const std::vector<uint8_t>& outputValues) override;

private:
   std::mutex m_numberUpdateLocker;
   int m_number;
   UMXDevice* m_dev;

   std::chrono::steady_clock::time_point m_lastTime;
   bool m_lastTimeInit;
   static const int MinDataLineDelayMs = 5;
   int m_longestDataLineDelayMs;
};

}
