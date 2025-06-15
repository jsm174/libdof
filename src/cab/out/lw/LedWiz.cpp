#include "LedWiz.h"
#include "../../../Log.h"
#include "../../../general/MathExtensions.h"
#include "../../../general/StringExtensions.h"

#include <cstring>
#include <thread>
#include <stdexcept>

namespace DOF
{

LedWiz::LedWiz() { InitializeDefaults(); }

LedWiz::LedWiz(int number)
   : LedWiz()
{
   SetNumber(number);
}

LedWiz::~LedWiz() { DisconnectFromController(); }

void LedWiz::InitializeDefaults()
{
   m_number = 1;
   m_comPort = "";
   m_baudRate = 9600;
   m_minCommandIntervalMs = 10;
   m_serialPort = nullptr;
   m_connected = false;
   m_lastUpdate = std::chrono::steady_clock::now();
   m_outputsChanged = false;


   std::memset(m_lastOutputValues, 0, sizeof(m_lastOutputValues));
   std::memset(m_lastSwitchStates, 0, sizeof(m_lastSwitchStates));
   m_updateInProgress = false;

   SetName(GenerateDeviceName());
}

void LedWiz::SetNumber(int number)
{
   if (number < 1 || number > 16)
   {
      Log::Warning(StringExtensions::Build("LedWiz unit numbers must be between 1-16. The supplied number {0} is out of range.", std::to_string(number)));
      return;
   }

   m_number = number;
   SetName(GenerateDeviceName());
}

void LedWiz::SetMinCommandIntervalMs(int intervalMs) { m_minCommandIntervalMs = MathExtensions::Limit(intervalMs, 0, 1000); }

void LedWiz::SetComPort(const std::string& comPort)
{
   if (m_connected && m_comPort != comPort)
   {
      Log::Write(StringExtensions::Build("LedWiz {0}: Changing COM port from {1} to {2}", std::to_string(m_number), m_comPort, comPort));
      DisconnectFromController();
   }

   m_comPort = comPort;
}

void LedWiz::SetBaudRate(int baudRate) { m_baudRate = baudRate; }

std::string LedWiz::GenerateDeviceName() const { return "LedWiz " + std::to_string(m_number) + (m_comPort.empty() ? "" : " @" + m_comPort); }

void LedWiz::Init(Cabinet* pCabinet)
{
   Log::Write(StringExtensions::Build("Initializing LedWiz {0}", std::to_string(m_number)));
   OutputControllerCompleteBase::Init(pCabinet);
}

void LedWiz::Finish()
{
   Log::Write(StringExtensions::Build("Finishing LedWiz {0}", std::to_string(m_number)));
   AllOff();
   DisconnectFromController();
   OutputControllerCompleteBase::Finish();
}

void LedWiz::Update() { OutputControllerCompleteBase::Update(); }

void LedWiz::ConnectToController()
{
   if (m_connected)
      return;

   if (m_comPort.empty())
   {
      Log::Write(StringExtensions::Build("LedWiz {0}: No COM port specified", std::to_string(m_number)));
      return;
   }

   if (!OpenSerialPort())
   {
      throw std::runtime_error(StringExtensions::Build("Failed to connect to LedWiz on {0}", m_comPort));
   }
}

void LedWiz::DisconnectFromController()
{
   if (m_connected)
   {
      Log::Write(StringExtensions::Build("LedWiz {0}: Disconnecting from {1}", std::to_string(m_number), m_comPort));
      CloseSerialPort();
   }
}

bool LedWiz::OpenSerialPort()
{
   sp_return result = sp_get_port_by_name(m_comPort.c_str(), &m_serialPort);
   if (result != SP_OK)
   {
      Log::Warning(StringExtensions::Build("LedWiz {0}: Failed to get port {1}: {2}", std::to_string(m_number), m_comPort, sp_last_error_message()));
      return false;
   }

   result = sp_open(m_serialPort, SP_MODE_WRITE);
   if (result != SP_OK)
   {
      Log::Warning(StringExtensions::Build("LedWiz {0}: Failed to open port {1}: {2}", std::to_string(m_number), m_comPort, sp_last_error_message()));
      sp_free_port(m_serialPort);
      m_serialPort = nullptr;
      return false;
   }

   sp_set_baudrate(m_serialPort, m_baudRate);
   sp_set_bits(m_serialPort, 8);
   sp_set_parity(m_serialPort, SP_PARITY_NONE);
   sp_set_stopbits(m_serialPort, 1);
   sp_set_flowcontrol(m_serialPort, SP_FLOWCONTROL_NONE);

   m_connected = true;
   return true;
}

void LedWiz::CloseSerialPort()
{
   if (m_serialPort)
   {
      sp_close(m_serialPort);
      sp_free_port(m_serialPort);
      m_serialPort = nullptr;
   }
   m_connected = false;
}

void LedWiz::UpdateOutputs(const std::vector<uint8_t>& outputValues)
{
   if (!m_connected || outputValues.empty())
      return;

   std::lock_guard<std::mutex> lock(m_updateMutex);
   if (m_updateInProgress)
      return;

   m_updateInProgress = true;


   bool hasChanges = false;
   size_t outputCount = std::min(outputValues.size(), static_cast<size_t>(32));

   for (size_t i = 0; i < outputCount; ++i)
   {
      if (m_lastOutputValues[i] != outputValues[i])
      {
         hasChanges = true;
         m_lastOutputValues[i] = outputValues[i];
      }
   }

   if (hasChanges)
   {
      UpdateLedWizOutputs();
   }

   m_updateInProgress = false;
}

void LedWiz::UpdateLedWizOutputs()
{


   uint8_t switchData[4];
   uint8_t pwmValues[32];

   ConvertToSwitchStates(m_lastOutputValues, switchData);
   ConvertToPWMValues(m_lastOutputValues, pwmValues);


   if (!SendSBACommand(switchData))
   {
      Log::Warning(StringExtensions::Build("LedWiz {0}: Failed to send SBA command", std::to_string(m_number)));
      return;
   }


   for (int bank = 0; bank < 4; ++bank)
   {
      uint8_t startOutput = bank * 8;
      if (!SendPBACommand(startOutput, &pwmValues[startOutput]))
      {
         Log::Warning(StringExtensions::Build("LedWiz {0}: Failed to send PBA command for bank {1}", std::to_string(m_number), std::to_string(bank)));
         return;
      }
   }


   SendSBACommand(switchData);
}

void LedWiz::AllOff()
{
   if (!m_connected)
      return;


   uint8_t command[33];
   command[0] = 0x40 + (m_number - 1);
   std::memset(&command[1], 0, 32);

   SendCommand(command, sizeof(command));
   std::memset(m_lastOutputValues, 0, sizeof(m_lastOutputValues));
}

bool LedWiz::SendCommand(const uint8_t* data, size_t length)
{
   if (!m_connected || !m_serialPort)
      return false;

   sp_return result = sp_blocking_write(m_serialPort, data, length, 1000);
   if (result < 0)
   {
      Log::Warning(StringExtensions::Build("LedWiz {0}: Serial write failed: {1}", std::to_string(m_number), sp_last_error_message()));
      return false;
   }

   if (static_cast<size_t>(result) != length)
   {
      Log::Warning(StringExtensions::Build("LedWiz {0}: Incomplete write ({1} of {2} bytes)", std::to_string(m_number), std::to_string(result), std::to_string(length)));
      return false;
   }

   return true;
}

bool LedWiz::VerifySettings()
{
   if (m_number < 1 || m_number > 16)
      return false;

   if (m_comPort.empty())
      return false;

   if (m_baudRate <= 0)
      return false;

   return true;
}

void LedWiz::FindDevices() { Log::Write("LedWiz: Device discovery not implemented - manually configure COM ports"); }

std::vector<int> LedWiz::GetLedwizNumbers()
{
   std::vector<int> numbers;
   return numbers;
}

std::vector<std::string> LedWiz::GetAvailableComPorts()
{
   std::vector<std::string> ports;

   struct sp_port** portList;
   sp_return result = sp_list_ports(&portList);

   if (result == SP_OK)
   {
      for (int i = 0; portList[i] != nullptr; i++)
      {
         const char* portName = sp_get_port_name(portList[i]);
         if (portName)
            ports.push_back(std::string(portName));
      }

      sp_free_port_list(portList);
   }

   return ports;
}

XMLElement* LedWiz::ToXml(XMLDocument& doc) const
{
   XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   if (!GetName().empty())
      element->SetAttribute("Name", GetName().c_str());

   element->SetAttribute("Number", m_number);

   if (!m_comPort.empty())
      element->SetAttribute("ComPort", m_comPort.c_str());

   element->SetAttribute("BaudRate", m_baudRate);
   element->SetAttribute("MinCommandIntervalMs", m_minCommandIntervalMs);

   return element;
}

bool LedWiz::FromXml(const XMLElement* element)
{
   if (!element)
      return false;

   const char* name = element->Attribute("Name");
   if (name)
      SetName(name);

   int number = m_number;
   element->QueryIntAttribute("Number", &number);
   SetNumber(number);

   const char* comPort = element->Attribute("ComPort");
   if (comPort)
      SetComPort(comPort);

   int baudRate = m_baudRate;
   element->QueryIntAttribute("BaudRate", &baudRate);
   SetBaudRate(baudRate);

   int minInterval = m_minCommandIntervalMs;
   element->QueryIntAttribute("MinCommandIntervalMs", &minInterval);
   SetMinCommandIntervalMs(minInterval);

   return true;
}


bool LedWiz::SendSBACommand(const uint8_t switchData[4])
{
   uint8_t command[9];
   command[0] = 0x40 + (m_number - 1);
   command[1] = switchData[0];
   command[2] = switchData[1];
   command[3] = switchData[2];
   command[4] = switchData[3];
   command[5] = 0;
   command[6] = 0;
   command[7] = 0;
   command[8] = 0;

   return SendCommand(command, sizeof(command));
}

bool LedWiz::SendPBACommand(uint8_t startOutput, const uint8_t pwmData[8])
{
   uint8_t command[9];
   command[0] = 0x20 + (m_number - 1);

   for (int i = 0; i < 8; ++i)
      command[i + 1] = pwmData[i];

   return SendCommand(command, sizeof(command));
}

void LedWiz::ConvertToPWMValues(const uint8_t input[32], uint8_t output[32]) const
{
   for (int i = 0; i < 32; ++i)
   {
      if (input[i] == 0)
         output[i] = 0;
      else
         output[i] = 1 + ((input[i] - 1) * 48) / 254;
   }
}

void LedWiz::ConvertToSwitchStates(const uint8_t input[32], uint8_t output[4]) const
{
   std::memset(output, 0, 4);

   for (int i = 0; i < 32; ++i)
   {
      if (input[i] > 0)
      {
         int byteIndex = i / 8;
         int bitIndex = i % 8;
         output[byteIndex] |= (1 << bitIndex);
      }
   }
}

}