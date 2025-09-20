#pragma once

#include "../OutputControllerCompleteBase.h"
#include <hidapi/hidapi.h>
#include <string>
#include <chrono>

namespace DOF
{

class LedWiz : public OutputControllerCompleteBase
{
public:
   LedWiz();
   LedWiz(int number);
   virtual ~LedWiz();

   int GetNumber() const { return m_number; }
   void SetNumber(int value);
   int GetMinCommandIntervalMs() const { return m_minCommandIntervalMs; }
   void SetMinCommandIntervalMs(int value);
   virtual void Init(Cabinet* cabinet) override;
   virtual void Finish() override;
   virtual void Update() override;
   bool IsConnected() const;
   void AllOff();
   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "LedWiz"; }

   static void FindDevices();
   static std::vector<int> GetLedwizNumbers();

protected:
   virtual int GetNumberOfConfiguredOutputs() override { return 32; }


   virtual bool VerifySettings() override;
   virtual void ConnectToController() override;
   virtual void DisconnectFromController() override;
   virtual void UpdateOutputs(const std::vector<uint8_t>& outputValues) override;

private:
   struct LWDEVICE
   {
      LWDEVICE(int unitNo, const std::string& path, const std::string& productName)
         : unitNo(unitNo)
         , path(path)
         , productName(productName)
      {
      }

      int unitNo;
      std::string path;
      std::string productName;
   };

   static std::string GetDeviceProductName(hid_device_info* dev);
   static std::string GetDeviceManufacturerName(hid_device_info* dev);
   bool WriteUSB(const std::vector<uint8_t>& data);

   int m_number;
   int m_minCommandIntervalMs;
   bool m_minCommandIntervalMsSet;
   std::vector<uint8_t> m_oldOutputValues;
   std::chrono::steady_clock::time_point m_lastUpdate;
   static std::vector<LWDEVICE> s_deviceList;
   hid_device* m_fp;
   std::string m_path;


   void AddOutputs();
};

}