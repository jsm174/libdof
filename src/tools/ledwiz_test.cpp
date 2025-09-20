#include <hidapi/hidapi.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <regex>
#include <string>

const uint16_t LEDWIZ_VENDOR_ID = 0xFAFA;
const uint16_t ZEBSBOARDS_VENDOR_ID = 0x20A0;

struct LedWizDevice
{
   int unitNo;
   std::string path;
   std::string productName;
   uint16_t vendorId;
   uint16_t productId;
};

class LedWizDirectTest
{
private:
   hid_device* m_deviceHandle;
   std::vector<uint8_t> m_outputValues;
   std::vector<uint8_t> m_oldOutputValues;
   LedWizDevice m_device;

public:
   LedWizDirectTest()
      : m_deviceHandle(nullptr)
   {
      m_outputValues.resize(32, 0);
      m_oldOutputValues.resize(32, 255); // Initialize to 255 to match C# exactly
   }

   ~LedWizDirectTest() { Disconnect(); }

   std::vector<LedWizDevice> FindDevices()
   {
      std::vector<LedWizDevice> devices;

      if (hid_init() != 0)
      {
         std::cout << "ERROR: Failed to initialize HIDAPI" << std::endl;
         return devices;
      }

      hid_device_info* devs = hid_enumerate(0x0000, 0x0000);
      hid_device_info* curDev = devs;

      while (curDev)
      {
         std::string productName;
         std::string manufacturerName;

         if (curDev->product_string)
         {
            std::wstring wproduct(curDev->product_string);
            productName = std::string(wproduct.begin(), wproduct.end());
         }

         if (curDev->manufacturer_string)
         {
            std::wstring wmanufacturer(curDev->manufacturer_string);
            manufacturerName = std::string(wmanufacturer.begin(), wmanufacturer.end());
         }


         bool ok = false;
         std::string okBecause;

         if (curDev->vendor_id == LEDWIZ_VENDOR_ID)
         {
            ok = true;
            okBecause = "recognized by LedWiz vendor ID";
         }
         else if (curDev->vendor_id == ZEBSBOARDS_VENDOR_ID)
         {
            std::regex zebsPattern("zebsboards", std::regex::ECMAScript | std::regex::icase);
            if (std::regex_search(manufacturerName, zebsPattern))
            {
               ok = true;
               okBecause = "recognized by ZebsBoards manufacturer";
            }
         }

         if (ok)
         {
            int unitNo = (curDev->product_id & 0x0F) + 1;
            if (unitNo < 1 || unitNo > 16)
               unitNo = 1;

            std::cout << "Found LedWiz device: VID:0x" << std::hex << curDev->vendor_id << " PID:0x" << curDev->product_id << std::dec << " Unit:" << unitNo << " (" << okBecause << ")"
                      << std::endl;

            LedWizDevice device;
            device.unitNo = unitNo;
            device.path = curDev->path ? curDev->path : "";
            device.productName = productName;
            device.vendorId = curDev->vendor_id;
            device.productId = curDev->product_id;
            devices.push_back(device);
         }

         curDev = curDev->next;
      }

      hid_free_enumeration(devs);
      return devices;
   }

   bool Connect(const LedWizDevice& device)
   {
      m_device = device;

      m_deviceHandle = hid_open_path(device.path.c_str());
      if (!m_deviceHandle)
      {
         std::cout << "ERROR: Failed to open LedWiz device unit " << device.unitNo << std::endl;
         return false;
      }

      std::cout << "Successfully connected to LedWiz unit " << device.unitNo << std::endl;
      return true;
   }

   void Disconnect()
   {
      if (m_deviceHandle)
      {
         TurnOffAllOutputs();

         std::this_thread::sleep_for(std::chrono::milliseconds(100));

         hid_close(m_deviceHandle);
         m_deviceHandle = nullptr;
      }
   }

   bool WriteUSB(const std::vector<uint8_t>& data)
   {
      if (!m_deviceHandle || data.empty())
         return false;

      int result = hid_write(m_deviceHandle, data.data(), data.size());
      if (result < 0)
      {
         std::cout << "ERROR: LedWiz WriteUSB failed" << std::endl;
         return false;
      }

      return true;
   }

   bool UpdateOutputs()
   {
      bool hasChanges = false;
      for (int i = 0; i < 32; ++i)
      {
         if (m_outputValues[i] != m_oldOutputValues[i])
         {
            hasChanges = true;
            break;
         }
      }

      if (!hasChanges)
         return true;

      std::vector<uint8_t> sbaCmd = { 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

      for (int i = 0; i < 32; ++i)
      {
         int byteIndex = 2 + (i / 8);
         int bitIndex = i % 8;

         if (m_outputValues[i] > 127)
         {
            sbaCmd[byteIndex] |= (1 << bitIndex);
         }
      }

      if (!WriteUSB(sbaCmd))
         return false;

      for (int ofs = 0; ofs < 32; ofs += 8)
      {
         std::vector<uint8_t> pbaCmd = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

         for (int i = 0; i < 8; ++i)
         {
            int outputIndex = ofs + i;
            pbaCmd[1 + i] = static_cast<uint8_t>((m_outputValues[outputIndex] * 49) / 255);
         }

         if (!WriteUSB(pbaCmd))
            return false;
      }

      m_oldOutputValues = m_outputValues;
      return true;
   }

   void SetOutput(int outputIndex, uint8_t intensity)
   {
      if (outputIndex >= 0 && outputIndex < 32)
      {
         m_outputValues[outputIndex] = intensity;
      }
   }

   void TurnOffAllOutputs()
   {
      std::fill(m_outputValues.begin(), m_outputValues.end(), 0);
      UpdateOutputs();
   }

   void RunTest()
   {
      std::cout << "\nStarting LedWiz output test..." << std::endl;
      std::cout << "Controls:" << std::endl;
      std::cout << "  [Enter] or 'n' = Next output" << std::endl;
      std::cout << "  'r' = Repeat current output" << std::endl;
      std::cout << "  'q' = Quit" << std::endl;
      std::cout << "\nPress Enter to start...\n";
      std::cin.ignore(); // Clear any pending input
      std::cin.get(); // Wait for initial Enter

      int outputIndex = 0;
      while (outputIndex < 32)
      {
         std::cout << "\nTesting Output " << (outputIndex + 1) << " (0-based index: " << outputIndex << ")... " << std::flush;

         SetOutput(outputIndex, 255);
         if (!UpdateOutputs())
         {
            std::cout << "FAILED" << std::endl;
            outputIndex++;
            continue;
         }

         std::cout << "ON... " << std::flush;

         std::this_thread::sleep_for(std::chrono::milliseconds(1000));

         SetOutput(outputIndex, 0);
         if (!UpdateOutputs())
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
   std::cout << "LedWiz Direct HID Test Program" << std::endl;
   std::cout << "==============================" << std::endl;
   std::cout << "This program directly communicates with LedWiz devices via HID" << std::endl;
   std::cout << "Supports both original LedWiz (VID:0xFAFA) and ZebsBoards (VID:0x20A0)" << std::endl;

   LedWizDirectTest* tester = nullptr;

   try
   {
      tester = new LedWizDirectTest();

      std::vector<LedWizDevice> devices = tester->FindDevices();

      if (devices.empty())
      {
         std::cout << "\nNo LedWiz devices found!" << std::endl;
         std::cout << "\nTroubleshooting:" << std::endl;
         std::cout << "1. Check USB connection" << std::endl;
         std::cout << "2. Verify device permissions (you may need to run as root)" << std::endl;
         std::cout << "3. Make sure LedWiz is properly powered" << std::endl;
         std::cout << "4. Check for LedWiz VID:0xFAFA or ZebsBoards VID:0x20A0" << std::endl;
         delete tester;
         return 1;
      }

      LedWizDevice selectedDevice;
      if (devices.size() == 1)
      {
         selectedDevice = devices[0];
         std::cout << "\nUsing the only LedWiz device found (Unit " << selectedDevice.unitNo << ")" << std::endl;
      }
      else
      {
         std::cout << "\nMultiple LedWiz devices found:" << std::endl;
         for (size_t i = 0; i < devices.size(); ++i)
         {
            std::cout << (i + 1) << ". Unit " << devices[i].unitNo << " (VID:0x" << std::hex << devices[i].vendorId << " PID:0x" << devices[i].productId << std::dec << ")" << std::endl;
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

   hid_exit();
   return 0;
}