#pragma once

#include <hidapi/hidapi.h>

#include "../OutputControllerFlexCompleteBase.h"
#include "DOF/DOF.h"

namespace DOF
{

class PinscapeDevice;

class Pinscape : public OutputControllerFlexCompleteBase
{
public:
   static void Initialize();

   Pinscape();
   Pinscape(int number);
   ~Pinscape();

   int GetNumber() { return m_number; }
   void SetNumber(int value);

   int GetMinCommandIntervalMs() const { return m_minCommandIntervalMs; }
   void SetMinCommandIntervalMs(int value);
   void Init(Cabinet* pCabinet) override;
   void Finish() override;

   virtual bool VerifySettings() override;
   virtual void ConnectToController() override;
   virtual void DisconnectFromController() override;
   virtual void UpdateOutputs(const std::vector<uint8_t>& outputValues) override;

protected:
   virtual int GetNumberOfConfiguredOutputs() override;

public:
   int GetNumberOfOutputs() const;


   void AllOff();
   void UpdateDelay();
   static void FindDevices();
   static std::vector<PinscapeDevice*> AllDevices() { return m_devices; }
   static std::vector<PinscapeDevice*> GetAllDevices() { return m_devices; }


   virtual XMLElement* ToXml(XMLDocument& doc) const override;
   virtual bool FromXml(const XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "Pinscape"; }

private:
   static std::string GetProductName(hid_device_info* dev);

   int m_number;
   int m_minCommandIntervalMs;
   bool m_minCommandIntervalMsSet;
   std::vector<uint8_t> m_oldOutputValues;
   std::chrono::steady_clock::time_point m_lastUpdate;
   static std::vector<PinscapeDevice*> m_devices;
   PinscapeDevice* m_pDevice;
};

}
