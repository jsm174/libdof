#pragma once

#include "../OutputControllerCompleteBase.h"
#include <libserialport.h>
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
   void SetNumber(int number);

   int GetMinCommandIntervalMs() const { return m_minCommandIntervalMs; }
   void SetMinCommandIntervalMs(int intervalMs);

   const std::string& GetComPort() const { return m_comPort; }
   void SetComPort(const std::string& comPort);

   int GetBaudRate() const { return m_baudRate; }
   void SetBaudRate(int baudRate);


   virtual void Init(Cabinet* pCabinet) override;
   virtual void Finish() override;
   virtual void Update() override;


   bool IsConnected() const { return m_connected; }


   void AllOff();


   bool SendSBACommand(const uint8_t switchData[4]);
   bool SendPBACommand(uint8_t startOutput, const uint8_t pwmData[8]);
   void UpdateLedWizOutputs();


   virtual XMLElement* ToXml(XMLDocument& doc) const override;
   virtual bool FromXml(const XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "LedWiz"; }


   static void FindDevices();
   static std::vector<std::string> GetAvailableComPorts();
   static std::vector<int> GetLedwizNumbers();

protected:
   virtual int GetNumberOfConfiguredOutputs() override { return 32; }


   virtual bool VerifySettings() override;
   virtual void ConnectToController() override;
   virtual void DisconnectFromController() override;
   virtual void UpdateOutputs(const std::vector<uint8_t>& outputValues) override;

private:
   int m_number;
   std::string m_comPort;
   int m_baudRate;
   int m_minCommandIntervalMs;


   struct sp_port* m_serialPort;
   bool m_connected;
   std::chrono::steady_clock::time_point m_lastUpdate;


   uint8_t m_lastOutputValues[32];
   uint8_t m_lastSwitchStates[32];
   bool m_outputsChanged;


   mutable std::mutex m_updateMutex;
   bool m_updateInProgress;


   bool OpenSerialPort();
   void CloseSerialPort();
   bool SendCommand(const uint8_t* data, size_t length);
   void InitializeDefaults();
   std::string GenerateDeviceName() const;


   void ConvertToPWMValues(const uint8_t input[32], uint8_t output[32]) const;
   void ConvertToSwitchStates(const uint8_t input[32], uint8_t output[4]) const;
};

}