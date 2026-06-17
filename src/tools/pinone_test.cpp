#include <libserialport.h>

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdint>

std::string TestSerialPort(const char* portName)
{
   std::cout << "\n=== Probing " << portName << " ===" << std::endl;

   struct sp_port* port = nullptr;

   if (sp_get_port_by_name(portName, &port) != SP_OK)
   {
      std::cout << "  sp_get_port_by_name FAILED" << std::endl;
      return "";
   }

   if (sp_open(port, SP_MODE_READ_WRITE) != SP_OK)
   {
      std::cout << "  sp_open FAILED (in use, or no permission)" << std::endl;
      sp_free_port(port);
      return "";
   }
   std::cout << "  sp_open OK" << std::endl;

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
   int written = sp_blocking_write(port, command, 9, 100);
   std::cout << "  wrote " << written << " bytes (expected 9)" << std::endl;

   char buffer[256];
   int bytesRead = sp_blocking_read(port, buffer, sizeof(buffer) - 1, 100);
   std::cout << "  read " << bytesRead << " bytes" << std::endl;

   std::string result;
   if (bytesRead > 0)
   {
      buffer[bytesRead] = '\0';

      std::cout << "  hex:   ";
      for (int i = 0; i < bytesRead; i++)
         std::cout << std::hex << ((buffer[i] >> 4) & 0xF) << (buffer[i] & 0xF) << " ";
      std::cout << std::dec << std::endl;

      result = std::string(buffer);
      while (!result.empty() && (result.back() == '\r' || result.back() == '\n'))
         result.pop_back();

      std::cout << "  trimmed string: \"" << result << "\"" << std::endl;
   }

   sp_flush(port, SP_BUF_BOTH);
   sp_set_rts(port, SP_RTS_OFF);
   sp_set_dtr(port, SP_DTR_OFF);
   sp_close(port);
   sp_free_port(port);

   std::this_thread::sleep_for(std::chrono::milliseconds(10));

   if (result == "DEBUG,CSD Board Connected")
   {
      std::cout << "  >>> PinOne DETECTED on " << portName << std::endl;
      return std::string(portName);
   }

   std::cout << "  not a PinOne" << std::endl;
   return "";
}

int main(int argc, char* argv[])
{
   std::cout << "PinOne Serial Detection Test" << std::endl;
   std::cout << "============================" << std::endl;
   std::cout << "libserialport version: " << sp_get_lib_version_string() << std::endl;

   if (argc > 1)
   {
      std::string found = TestSerialPort(argv[1]);
      return found.empty() ? 1 : 0;
   }

   struct sp_port** portList;
   if (sp_list_ports(&portList) != SP_OK)
   {
      std::cout << "sp_list_ports FAILED - no serial ports visible" << std::endl;
      return 1;
   }

   std::cout << "\nPorts found:" << std::endl;
   for (int i = 0; portList[i] != nullptr; i++)
   {
      char* name = sp_get_port_name(portList[i]);
      char* desc = sp_get_port_description(portList[i]);
      std::cout << "  " << (name ? name : "(null)") << "  -  " << (desc ? desc : "") << std::endl;
   }

   std::string found;
   for (int i = 0; portList[i] != nullptr; i++)
   {
      char* name = sp_get_port_name(portList[i]);
      if (name)
      {
         found = TestSerialPort(name);
         if (!found.empty())
            break;
      }
   }

   sp_free_port_list(portList);

   std::cout << "\n----------------------------" << std::endl;
   if (found.empty())
      std::cout << "RESULT: no PinOne detected" << std::endl;
   else
      std::cout << "RESULT: PinOne on " << found << std::endl;

   return found.empty() ? 1 : 0;
}
