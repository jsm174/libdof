#pragma once

#include "../OutputControllerFlexCompleteBase.h"
#include <string>
#include <vector>
#include <mutex>
#include <chrono>

#ifdef __HIDAPI__
#include <hidapi/hidapi.h>
#endif

namespace DOF
{

class Cabinet;

class DudesCab : public OutputControllerFlexCompleteBase
{
public:
   static const uint16_t VendorID = 0x2E8A;
   static const uint16_t ProductID = 0x106F;

   DudesCab();
   DudesCab(int number);
   virtual ~DudesCab();

   int GetNumber() const { return m_number; }
   void SetNumber(int value);

   int GetMinCommandIntervalMs() const { return m_minCommandIntervalMs; }
   void SetMinCommandIntervalMs(int value);

   virtual void Init(Cabinet* cabinet) override;
   virtual void Finish() override;

   virtual std::string GetXmlElementName() const override { return "DudesCab"; }

   class Device
   {
   public:
      enum HIDReportType
      {
         RT_HANDSHAKE = 1,
         RT_INFOS,
         RT_PWM_GETEXTENSIONSINFOS,
         RT_PWM_ALLOFF,
         RT_PWM_OUTPUTS,
         RT_MX_GETEXTENSIONSINFOS,
         RT_MX_ALLOFF,
         RT_MX_OUTPUTS,
         RT_MAX
      };

      Device(const std::string& path, const std::string& name, const std::string& serial, uint16_t vendorID, uint16_t productID, uint16_t version);
      ~Device();

      int UnitNo() const { return m_unitNo; }
      int NumOutputs() const { return m_numOutputs; }
      const std::string& GetSerial() const { return m_serial; }
      std::string ToString() const;

      void AllOff();
      void SendCommand(HIDReportType command, const std::vector<uint8_t>& parameters = {});

      uint8_t PwmMaxOutputsPerExtension;
      uint8_t PwmExtensionsMask;

   private:
      std::string m_path;
      std::string m_name;
      std::string m_serial;
      uint16_t m_vendorID;
      uint16_t m_productID;
      uint16_t m_version;
      int m_unitNo;
      int m_numOutputs;
      int m_maxExtensions;

#ifdef __HIDAPI__
      hid_device* m_hidDevice;
#else
      void* m_hidDevice;
#endif

      std::vector<uint8_t> ReadUSB();
      bool WriteUSB(const std::vector<uint8_t>& data);

      static const int HID_COMMAND_PREFIX_SIZE = 5;
      static const uint8_t RID_OUTPUTS = 3;
   };

   static std::vector<Device*> AllDevices();

protected:
   virtual bool VerifySettings() override;
   virtual void UpdateOutputs(const std::vector<uint8_t>& newOutputValues) override;
   virtual void ConnectToController() override;
   virtual void DisconnectFromController() override;

private:
   std::mutex m_numberUpdateLocker;
   int m_number;
   int m_minCommandIntervalMs;
   bool m_minCommandIntervalMsSet;

   Device* m_dev;
   std::vector<uint8_t> m_oldOutputValues;
   std::vector<uint8_t> m_outputBuffer;

   std::chrono::steady_clock::time_point m_lastUpdate;
   void UpdateDelay();
   void Instrumentation(const std::string& message);

   static std::vector<Device*> FindDevices();
   static std::vector<Device*> s_devices;
};

}