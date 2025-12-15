#include "PinOneAutoConfigurator.h"
#include "PinOne.h"
#include "PinOneCommunication.h"
#include "../../Cabinet.h"
#include "../OutputControllerList.h"
#include "../../../general/StringExtensions.h"
#include "../../../Log.h"
#include "../../toys/lwequivalent/LedWizEquivalent.h"
#include "../../toys/ToyList.h"

#include <libserialport.h>
#include <algorithm>
#include <thread>
#include <chrono>
#include <fstream>

namespace DOF
{

void PinOneAutoConfigurator::AutoConfig(Cabinet* cabinet)
{
   Log::Write("PinOne auto-configuration starting");

   const int UnitBias = 10;

   std::vector<std::string> preconfigured;
   for (auto* controller : *cabinet->GetOutputControllers())
   {
      PinOne* pinOne = dynamic_cast<PinOne*>(controller);
      if (pinOne)
         preconfigured.push_back(pinOne->GetComPort());
   }

   std::string comPort = GetDevice();

   if (std::find(preconfigured.begin(), preconfigured.end(), comPort) == preconfigured.end() && !comPort.empty())
   {
      PinOne* p = new PinOne(comPort);
      if (!cabinet->GetOutputControllers()->Contains(p->GetName()))
      {
         cabinet->GetOutputControllers()->push_back(p);
         Log::Write(StringExtensions::Build("Detected and added PinOne Controller Nr. {0} with name {1}", std::to_string(p->GetNumber()), p->GetName()));

         bool foundEquivalent = false;
         for (auto* toy : *cabinet->GetToys())
         {
            LedWizEquivalent* lwe = dynamic_cast<LedWizEquivalent*>(toy);
            if (lwe && lwe->GetLedWizNumber() == p->GetNumber() + UnitBias)
            {
               foundEquivalent = true;
               break;
            }
         }

         if (!foundEquivalent)
         {
            LedWizEquivalent* lwe = new LedWizEquivalent();
            lwe->SetLedWizNumber(p->GetNumber() + UnitBias);
            lwe->SetName(StringExtensions::Build("{0} Equivalent", p->GetName()));

            for (int i = 1; i <= p->GetNumberOfOutputs(); i++)
            {
               LedWizEquivalentOutput* lweo = new LedWizEquivalentOutput();
               lweo->SetOutputName(StringExtensions::Build("{0}\\{0}.{1:00}", p->GetName(), std::to_string(i)));
               lweo->SetLedWizEquivalentOutputNumber(i);
               lwe->GetOutputs().AddOutput(lweo);
            }

            if (!cabinet->GetToys()->Contains(lwe->GetName()))
            {
               cabinet->GetToys()->push_back(lwe);
               Log::Write(StringExtensions::Build("Added LedwizEquivalent Nr. {0} with name {1} for PinOne Controller Nr. {2}, {3}", std::to_string(lwe->GetLedWizNumber()), lwe->GetName(),
                  std::to_string(p->GetNumber()), std::to_string(p->GetNumberOfOutputs())));
            }
         }
      }
   }
}

std::string PinOneAutoConfigurator::TestSerialPort(const char* portName)
{
   struct sp_port* port = nullptr;

   try
   {
      if (sp_get_port_by_name(portName, &port) != SP_OK)
      {
         Log::Write(StringExtensions::Build("PinOne TestSerialPort: sp_get_port_by_name failed for {0}", std::string(portName)));
         return "";
      }

      std::string portPath(portName);
      std::ifstream portFile(portPath);
      if (!portFile.good())
      {
         Log::Write(StringExtensions::Build("PinOne TestSerialPort: Port file does not exist: {0}", portPath));
         sp_free_port(port);
         return "";
      }
      portFile.close();

      if (sp_open(port, SP_MODE_READ_WRITE) != SP_OK)
      {
         Log::Write(StringExtensions::Build("PinOne TestSerialPort: sp_open failed for {0}", std::string(portName)));
         sp_free_port(port);
         return "";
      }

      Log::Write(StringExtensions::Build("PinOne TestSerialPort: Opened port {0}, configuring...", std::string(portName)));

      sp_set_baudrate(port, 2000000);
      sp_set_bits(port, 8);
      sp_set_parity(port, SP_PARITY_NONE);
      sp_set_stopbits(port, 1);
      sp_set_dtr(port, SP_DTR_ON);
      sp_set_rts(port, SP_RTS_OFF);
      sp_set_cts(port, SP_CTS_IGNORE);
      sp_set_dsr(port, SP_DSR_IGNORE);
      sp_set_xon_xoff(port, SP_XONXOFF_DISABLED);

      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      sp_flush(port, SP_BUF_BOTH);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      uint8_t command[] = { 0, 251, 0, 0, 0, 0, 0, 0, 0 };
      Log::Write(StringExtensions::Build("PinOne TestSerialPort: Sending detection command to {0}", std::string(portName)));
      sp_blocking_write(port, command, 9, 100);

      char buffer[256];
      int bytesRead = sp_blocking_read(port, buffer, sizeof(buffer) - 1, 100);

      Log::Write(StringExtensions::Build("PinOne TestSerialPort: Read {0} bytes from {1}", std::to_string(bytesRead), std::string(portName)));

      if (bytesRead > 0)
      {
         buffer[bytesRead] = '\0';
         std::string result(buffer);

         std::string hexDump;
         for (int i = 0; i < bytesRead; i++)
            hexDump += StringExtensions::Build("{0} ", std::to_string((unsigned char)buffer[i]));
         Log::Write(StringExtensions::Build("PinOne TestSerialPort: Raw bytes: {0}", hexDump));
         Log::Write(StringExtensions::Build("PinOne TestSerialPort: Response string: [{0}]", result));

         while (!result.empty() && (result.back() == '\r' || result.back() == '\n'))
            result.pop_back();

         Log::Write(StringExtensions::Build("PinOne TestSerialPort: After stripping line endings: [{0}]", result));

         if (result == "DEBUG,CSD Board Connected")
         {
            Log::Write(StringExtensions::Build("PinOne TestSerialPort: SUCCESS - PinOne detected on {0}", std::string(portName)));
            sp_close(port);
            sp_free_port(port);
            return std::string(portName);
         }
         else
         {
            Log::Write("PinOne TestSerialPort: Response does not match expected string");
         }
      }
      else
      {
         Log::Write(StringExtensions::Build("PinOne TestSerialPort: No response from {0}", std::string(portName)));
      }

      sp_flush(port, SP_BUF_BOTH);
      sp_set_rts(port, SP_RTS_OFF);
      sp_set_dtr(port, SP_DTR_OFF);

      sp_close(port);
      sp_free_port(port);
   }
   catch (...)
   {
      Log::Write(StringExtensions::Build("PinOne TestSerialPort: Exception while testing {0}", std::string(portName)));
      if (port)
      {
         try
         {
            sp_flush(port, SP_BUF_BOTH);
            sp_set_rts(port, SP_RTS_OFF);
            sp_set_dtr(port, SP_DTR_OFF);
         }
         catch (...)
         {
         }

         sp_close(port);
         sp_free_port(port);
      }
   }

   std::this_thread::sleep_for(std::chrono::milliseconds(10));

   return "";
}

std::string PinOneAutoConfigurator::GetDevice()
{
   Log::Write("PinOne GetDevice: Starting serial port enumeration");

   struct sp_port** portList;
   if (sp_list_ports(&portList) != SP_OK)
   {
      Log::Write("PinOne GetDevice: sp_list_ports failed");
      return "";
   }

   int portCount = 0;
   while (portList[portCount] != nullptr)
      portCount++;

   Log::Write(StringExtensions::Build("PinOne GetDevice: Found {0} serial ports", std::to_string(portCount)));

   for (int i = 0; portList[i] != nullptr; i++)
   {
      char* portName = sp_get_port_name(portList[i]);
      if (!portName)
         continue;

      Log::Write(StringExtensions::Build("PinOne GetDevice: Testing port {0}", std::string(portName)));

      std::string result = TestSerialPort(portName);
      if (!result.empty())
      {
         Log::Write(StringExtensions::Build("PinOne GetDevice: Found PinOne on {0}", result));
         sp_free_port_list(portList);
         return result;
      }
   }

   sp_free_port_list(portList);

   Log::Write("PinOne GetDevice: No PinOne found on serial ports, trying named pipe server");

   std::string comPort = "";
   PinOneCommunication communication("");
   if (communication.ConnectToServer())
   {
      comPort = communication.GetCOMPort();
      Log::Write(StringExtensions::Build("PinOne GetDevice: Got COM port from server: {0}", comPort));
   }
   else
   {
      Log::Write("PinOne GetDevice: Named pipe server connection failed");
   }

   return comPort;
}

}