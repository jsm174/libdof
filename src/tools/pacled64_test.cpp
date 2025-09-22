#include <libusb-1.0/libusb.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>

const uint16_t PACLED64_VENDOR_ID = 0xD209;
const uint16_t PACLED64_PRODUCT_ID = 0x1401; // Updated to match your device

class PacLed64DirectTest
{
private:
   libusb_context* m_context;
   libusb_device_handle* m_deviceHandle;
   std::vector<uint8_t> m_ledValues;

public:
   PacLed64DirectTest()
      : m_context(nullptr)
      , m_deviceHandle(nullptr)
   {
      m_ledValues.resize(64, 0);
   }

   ~PacLed64DirectTest() { Disconnect(); }

   bool Connect()
   {
      int result = libusb_init(&m_context);
      if (result < 0)
      {
         std::cout << "ERROR: Failed to initialize libusb: " << libusb_error_name(result) << std::endl;
         return false;
      }

      m_deviceHandle = libusb_open_device_with_vid_pid(m_context, PACLED64_VENDOR_ID, PACLED64_PRODUCT_ID);
      if (!m_deviceHandle)
      {
         std::cout << "ERROR: PacLed64 device not found (VID:0x" << std::hex << PACLED64_VENDOR_ID << " PID:0x" << PACLED64_PRODUCT_ID << std::dec << ")" << std::endl;
         libusb_exit(m_context);
         return false;
      }

      if (libusb_kernel_driver_active(m_deviceHandle, 0) == 1)
      {
         if (libusb_detach_kernel_driver(m_deviceHandle, 0) != 0)
         {
            std::cout << "WARNING: Could not detach kernel driver" << std::endl;
         }
      }

      result = libusb_claim_interface(m_deviceHandle, 0);
      if (result < 0)
      {
         std::cout << "ERROR: Failed to claim interface: " << libusb_error_name(result) << std::endl;
         libusb_close(m_deviceHandle);
         libusb_exit(m_context);
         return false;
      }

      std::cout << "Successfully connected to PacLed64" << std::endl;
      return true;
   }

   void Disconnect()
   {
      if (m_deviceHandle)
      {
         TurnOffAllLeds();

         std::this_thread::sleep_for(std::chrono::milliseconds(100));

         libusb_release_interface(m_deviceHandle, 0);
         libusb_close(m_deviceHandle);
         m_deviceHandle = nullptr;
      }

      if (m_context)
      {
         libusb_exit(m_context);
         m_context = nullptr;
      }
   }

   bool SendLedUpdate()
   {
      if (!m_deviceHandle)
         return false;


      for (int i = 0; i < 64; i++)
      {
         uint8_t data[2] = { static_cast<uint8_t>(i), m_ledValues[i] };

         int result = libusb_control_transfer(m_deviceHandle,
            0x21, // bmRequestType
            9, // bRequest
            0x0200, // wValue
            0, // wIndex
            data,
            2, // wLength
            2000 // timeout
         );

         if (result < 0)
         {
            std::cout << "ERROR: Failed to send LED command for LED " << i << ": " << libusb_error_name(result) << std::endl;
            return false;
         }
      }

      return true;
   }

   void SetLed(int ledIndex, uint8_t intensity)
   {
      if (ledIndex >= 0 && ledIndex < 64)
      {
         m_ledValues[ledIndex] = intensity;
      }
   }

   void TurnOffAllLeds()
   {
      std::fill(m_ledValues.begin(), m_ledValues.end(), 0);
      SendLedUpdate();
   }

   char GetUserInput()
   {
      char ch;
      std::cin >> ch;
      return std::tolower(ch);
   }

   void RunTest()
   {
      std::cout << "\nStarting PacLed64 LED test..." << std::endl;
      std::cout << "Controls:" << std::endl;
      std::cout << "  [Enter] or 'n' = Next LED" << std::endl;
      std::cout << "  'r' = Repeat current LED" << std::endl;
      std::cout << "  'q' = Quit" << std::endl;
      std::cout << "\nPress Enter to start...\n";
      std::cin.get(); // Wait for initial Enter

      int ledIndex = 0;
      while (ledIndex < 64)
      {
         std::cout << "\nTesting LED " << (ledIndex + 1) << " (0-based index: " << ledIndex << ")... " << std::flush;

         SetLed(ledIndex, 255);
         if (!SendLedUpdate())
         {
            std::cout << "FAILED" << std::endl;
            ledIndex++;
            continue;
         }

         std::cout << "ON... " << std::flush;

         std::this_thread::sleep_for(std::chrono::milliseconds(1000));

         SetLed(ledIndex, 0);
         if (!SendLedUpdate())
         {
            std::cout << "FAILED TO TURN OFF" << std::endl;
            ledIndex++;
            continue;
         }

         std::cout << "OFF" << std::endl;
         std::cout << "Press Enter/n=next, r=repeat, q=quit: " << std::flush;

         char input;
         std::cin >> input;
         input = std::tolower(input);

         if (input == 'q')
         {
            std::cout << "Quitting test..." << std::endl;
            break;
         }
         else if (input == 'r')
         {
            std::cout << "Repeating LED " << (ledIndex + 1) << std::endl;
         }
         else
         {
            ledIndex++;
         }
      }

      TurnOffAllLeds();

      std::cout << "\nTest completed!" << std::endl;
      std::cout << "All LEDs should now be OFF" << std::endl;
   }
};

int main(int argc, char* argv[])
{
   std::cout << "PacLed64 Direct USB Test Program" << std::endl;
   std::cout << "=================================" << std::endl;
   std::cout << "This program directly communicates with PacLed64 via USB" << std::endl;
   std::cout << "Make sure you have permissions to access USB devices" << std::endl;
   std::cout << "(you may need to run as root or configure udev rules)" << std::endl;

   PacLed64DirectTest* tester = nullptr;

   try
   {
      tester = new PacLed64DirectTest();

      if (!tester->Connect())
      {
         std::cout << "\nTroubleshooting:" << std::endl;
         std::cout << "1. Check USB connection" << std::endl;
         std::cout << "2. Verify device permissions (try running as root)" << std::endl;
         std::cout << "3. Make sure PacLed64 is properly powered" << std::endl;
         std::cout << "4. Verify PID (found: 0x1401)" << std::endl;
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
