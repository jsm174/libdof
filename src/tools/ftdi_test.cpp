#include <libftdi1/ftdi.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <string>

const uint16_t FTDI_VENDOR_ID = 0x0403;
const uint16_t FTDI_PRODUCT_ID = 0x6001;

struct FtdiDevice
{
   std::string manufacturer;
   std::string description;
   std::string serialNumber;
};

class FT245RDirectTest
{
private:
   ftdi_context* m_ftdiContext;
   bool m_isOpen;
   uint8_t m_outputValue;
   FtdiDevice m_device;

public:
   FT245RDirectTest()
      : m_ftdiContext(nullptr)
      , m_isOpen(false)
      , m_outputValue(0)
   {
      m_ftdiContext = ftdi_new();
      if (!m_ftdiContext)
      {
         std::cout << "ERROR: Failed to initialize libftdi" << std::endl;
      }
   }

   ~FT245RDirectTest()
   {
      Disconnect();

      if (m_ftdiContext)
      {
         ftdi_free(m_ftdiContext);
         m_ftdiContext = nullptr;
      }
   }

   std::vector<FtdiDevice> FindDevices()
   {
      std::vector<FtdiDevice> devices;

      if (!m_ftdiContext)
         return devices;

      ftdi_device_list* devlist = nullptr;
      int result = ftdi_usb_find_all(m_ftdiContext, &devlist, FTDI_VENDOR_ID, FTDI_PRODUCT_ID);
      if (result < 0)
      {
         std::cout << "ERROR: Failed to enumerate FTDI devices: " << ftdi_get_error_string(m_ftdiContext) << std::endl;
         return devices;
      }

      for (ftdi_device_list* curDev = devlist; curDev; curDev = curDev->next)
      {
         char manufacturer[256] = { 0 };
         char description[256] = { 0 };
         char serial[256] = { 0 };

         if (ftdi_usb_get_strings(m_ftdiContext, curDev->dev, manufacturer, 256, description, 256, serial, 256) < 0)
         {
            std::cout << "WARNING: Could not read strings for an FTDI device: " << ftdi_get_error_string(m_ftdiContext) << std::endl;
            continue;
         }

         std::cout << "Found FTDI device: " << manufacturer << " " << description << " Serial:" << serial << std::endl;

         FtdiDevice device;
         device.manufacturer = manufacturer;
         device.description = description;
         device.serialNumber = serial;
         devices.push_back(device);
      }

      ftdi_list_free(&devlist);
      return devices;
   }

   bool Connect(const FtdiDevice& device)
   {
      m_device = device;

      int result = ftdi_usb_open_desc(m_ftdiContext, FTDI_VENDOR_ID, FTDI_PRODUCT_ID, nullptr, device.serialNumber.c_str());
      if (result < 0)
      {
         std::cout << "ERROR: Failed to open FTDI device " << device.serialNumber << ": " << ftdi_get_error_string(m_ftdiContext) << std::endl;
         return false;
      }

      m_isOpen = true;

      result = ftdi_set_bitmode(m_ftdiContext, 0xFF, 0x01);
      if (result < 0)
      {
         std::cout << "ERROR: Failed to set bitbang mode: " << ftdi_get_error_string(m_ftdiContext) << std::endl;
         Disconnect();
         return false;
      }

      std::cout << "Successfully connected to FTDI device " << device.serialNumber << std::endl;
      return true;
   }

   void Disconnect()
   {
      if (m_isOpen)
      {
         TurnOffAllOutputs();

         std::this_thread::sleep_for(std::chrono::milliseconds(100));

         ftdi_usb_close(m_ftdiContext);
         m_isOpen = false;
      }
   }

   bool SendUpdate()
   {
      if (!m_isOpen)
         return false;

      unsigned char out[1] = { m_outputValue };

      int result = ftdi_write_data(m_ftdiContext, out, 1);
      if (result != 1)
      {
         std::cout << "ERROR: FTDI write failed: " << ftdi_get_error_string(m_ftdiContext) << std::endl;
         return false;
      }

      return true;
   }

   void SetOutput(int outputIndex, bool on)
   {
      if (outputIndex >= 0 && outputIndex < 8)
      {
         if (on)
            m_outputValue |= (1 << outputIndex);
         else
            m_outputValue &= ~(1 << outputIndex);
      }
   }

   void TurnOffAllOutputs()
   {
      m_outputValue = 0;
      SendUpdate();
   }

   void RunTest()
   {
      std::cout << "\nStarting FT245R output test..." << std::endl;
      std::cout << "Controls:" << std::endl;
      std::cout << "  [Enter] or 'n' = Next output" << std::endl;
      std::cout << "  'r' = Repeat current output" << std::endl;
      std::cout << "  'q' = Quit" << std::endl;
      std::cout << "\nPress Enter to start...\n";
      std::cin.ignore();
      std::cin.get();

      int outputIndex = 0;
      while (outputIndex < 8)
      {
         std::cout << "\nTesting Output " << (outputIndex + 1) << " (0-based index: " << outputIndex << ")... " << std::flush;

         SetOutput(outputIndex, true);
         if (!SendUpdate())
         {
            std::cout << "FAILED" << std::endl;
            outputIndex++;
            continue;
         }

         std::cout << "ON... " << std::flush;

         std::this_thread::sleep_for(std::chrono::milliseconds(1000));

         SetOutput(outputIndex, false);
         if (!SendUpdate())
         {
            std::cout << "FAILED TO TURN OFF" << std::endl;
            outputIndex++;
            continue;
         }

         std::cout << "OFF" << std::endl;
         std::cout << "Press Enter/n=next, r=repeat, q=quit: " << std::flush;

         std::string input;
         std::getline(std::cin, input);

         if (!input.empty())
         {
            char firstChar = std::tolower(input[0]);
            if (firstChar == 'q')
            {
               std::cout << "Quitting test..." << std::endl;
               break;
            }
            else if (firstChar == 'r')
            {
               std::cout << "Repeating Output " << (outputIndex + 1) << std::endl;
               continue;
            }
         }

         outputIndex++;
      }

      TurnOffAllOutputs();

      std::cout << "\nTest completed!" << std::endl;
      std::cout << "All outputs should now be OFF" << std::endl;
   }
};

int main(int argc, char* argv[])
{
   std::cout << "FT245R Direct FTDI Test Program" << std::endl;
   std::cout << "===============================" << std::endl;
   std::cout << "This program directly communicates with FT245R bitbang boards via libftdi" << std::endl;
   std::cout << "Supports FTDI VID:0x0403 PID:0x6001 devices (e.g. Sainsmart 8-port relay boards)" << std::endl;
   std::cout << "Stop any cabinet software using the boards before running this test" << std::endl;

   FT245RDirectTest* tester = nullptr;

   try
   {
      tester = new FT245RDirectTest();

      std::vector<FtdiDevice> devices = tester->FindDevices();

      if (devices.empty())
      {
         std::cout << "\nNo FTDI devices found!" << std::endl;
         std::cout << "\nTroubleshooting:" << std::endl;
         std::cout << "1. Check USB connection" << std::endl;
         std::cout << "2. Verify device permissions (you may need to run as root or configure udev rules)" << std::endl;
         std::cout << "3. On Windows make sure the WinUSB driver is installed for the device" << std::endl;
         std::cout << "4. Check for FTDI VID:0x0403 PID:0x6001" << std::endl;
         delete tester;
         return 1;
      }

      FtdiDevice selectedDevice;
      if (devices.size() == 1)
      {
         selectedDevice = devices[0];
         std::cout << "\nUsing the only FTDI device found (Serial " << selectedDevice.serialNumber << ")" << std::endl;
      }
      else
      {
         std::cout << "\nMultiple FTDI devices found:" << std::endl;
         for (size_t i = 0; i < devices.size(); ++i)
         {
            std::cout << (i + 1) << ". Serial " << devices[i].serialNumber << " (" << devices[i].manufacturer << " " << devices[i].description << ")" << std::endl;
         }

         int choice = 0;
         while (choice < 1 || choice > static_cast<int>(devices.size()))
         {
            std::cout << "Select device (1-" << devices.size() << "): ";
            std::cin >> choice;
         }
         selectedDevice = devices[choice - 1];
      }

      if (!tester->Connect(selectedDevice))
      {
         delete tester;
         return 1;
      }

      tester->RunTest();
      delete tester;
   }
   catch (const std::exception& e)
   {
      std::cout << "ERROR: " << e.what() << std::endl;
      if (tester)
         delete tester;
      return 1;
   }
   catch (...)
   {
      std::cout << "ERROR: Unknown exception occurred" << std::endl;
      if (tester)
         delete tester;
      return 1;
   }

   return 0;
}
