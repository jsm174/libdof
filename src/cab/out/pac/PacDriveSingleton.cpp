#include "PacDriveSingleton.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include <string>
#include <algorithm>

namespace DOF
{

PacDriveSingleton& PacDriveSingleton::GetInstance()
{
   static PacDriveSingleton instance;
   return instance;
}

PacDriveSingleton::PacDriveSingleton()
   : m_numDevices(0)
   , m_libusbContext(nullptr)
{
   Initialize();
}

PacDriveSingleton::~PacDriveSingleton() { Shutdown(); }

void PacDriveSingleton::Initialize()
{
   int result = libusb_init(&m_libusbContext);
   if (result < 0)
   {
      Log::Exception(StringExtensions::Build("Failed to initialize libusb: {0}", std::to_string(result)));
      return;
   }

   EnumerateDevices();
}

void PacDriveSingleton::Shutdown()
{
   std::lock_guard<std::mutex> lock(m_hidDevicesMutex);
   for (auto& pair : m_hidDevices)
   {
      if (pair.second)
      {
         hid_close(pair.second);
      }
   }
   m_hidDevices.clear();

   {
      std::lock_guard<std::mutex> usbLock(m_usbDevicesMutex);
      for (auto& pair : m_usbDevices)
      {
         if (pair.second)
         {
            libusb_close(pair.second);
         }
      }
      m_usbDevices.clear();

      if (m_libusbContext)
      {
         libusb_exit(m_libusbContext);
         m_libusbContext = nullptr;
      }
   }
}

void PacDriveSingleton::EnumerateDevices()
{
   m_devices.clear();

   hid_device_info* devices = hid_enumerate(0x0, 0x0);
   hid_device_info* currentDevice = devices;

   int index = 0;
   while (currentDevice)
   {
      DeviceInfo info;
      info.vendorId = currentDevice->vendor_id;
      info.productId = currentDevice->product_id;
      info.path = currentDevice->path ? currentDevice->path : "";
      if (currentDevice->serial_number)
      {
         std::wstring wserial(currentDevice->serial_number);
         info.serial = std::string(wserial.begin(), wserial.end());
      }
      else
      {
         info.serial = "";
      }

      if (IsPacLed64Device(info.vendorId, info.productId))
      {
         info.type = DeviceType::PacLED64;
         info.deviceId = ExtractPacLed64Id(info.productId);
      }
      else if (IsPacDriveDevice(info.vendorId, info.productId))
      {
         info.type = DeviceType::PacDrive;
         info.deviceId = 1;
      }
      else if (IsPacUIODevice(info.vendorId, info.productId))
      {
         info.type = DeviceType::IPACIO;
         info.deviceId = ExtractPacUIOId(info.productId);
      }
      else
      {
         info.type = DeviceType::Unknown;
         info.deviceId = -1;
      }

      m_devices.push_back(info);
      currentDevice = currentDevice->next;
      index++;
   }

   hid_free_enumeration(devices);

   libusb_device** usbDevices;
   ssize_t deviceCount = libusb_get_device_list(m_libusbContext, &usbDevices);

   if (deviceCount > 0)
   {
      for (ssize_t i = 0; i < deviceCount; i++)
      {
         libusb_device* device = usbDevices[i];
         libusb_device_descriptor desc;

         if (libusb_get_device_descriptor(device, &desc) == 0)
         {
            if (IsPacLed64Device(desc.idVendor, desc.idProduct))
            {
               bool found = false;
               for (const auto& existing : m_devices)
               {
                  if (existing.vendorId == desc.idVendor && existing.productId == desc.idProduct && existing.type == DeviceType::PacLED64)
                  {
                     found = true;
                     break;
                  }
               }

               if (!found)
               {
                  DeviceInfo info;
                  info.type = DeviceType::PacLED64;
                  info.vendorId = desc.idVendor;
                  info.productId = desc.idProduct;
                  info.deviceId = ExtractPacLed64Id(desc.idProduct);
                  info.path = StringExtensions::Build("libusb:{0}:{1}", std::to_string(libusb_get_bus_number(device)), std::to_string(libusb_get_device_address(device)));
                  info.serial = "";

                  m_devices.push_back(info);
               }
            }
         }
      }
   }

   libusb_free_device_list(usbDevices, 1);

   m_numDevices = static_cast<int>(m_devices.size());
}

bool PacDriveSingleton::IsPacLed64Device(uint16_t vendorId, uint16_t productId) { return (vendorId == 0xD209) && (productId >= 0x1401) && (productId <= 0x1404); }

int PacDriveSingleton::ExtractPacLed64Id(uint16_t productId)
{
   if (productId >= 0x1401 && productId <= 0x1404)
   {
      return static_cast<int>(productId - 0x1401 + 1);
   }
   return -1;
}

std::vector<int> PacDriveSingleton::PacLed64GetIdList()
{
   std::vector<int> idList;

   EnumerateDevices();

   for (const auto& device : m_devices)
   {
      if (device.type == DeviceType::PacLED64 && device.deviceId > 0)
      {
         idList.push_back(device.deviceId);
      }
   }

   return idList;
}

int PacDriveSingleton::PacLed64GetIndexForDeviceId(int id)
{
   for (size_t i = 0; i < m_devices.size(); i++)
   {
      if (m_devices[i].type == DeviceType::PacLED64 && m_devices[i].deviceId == id)
      {
         return static_cast<int>(i);
      }
   }
   return -1;
}

int PacDriveSingleton::PacLed64GetDeviceId(int index)
{
   if (index >= 0 && index < static_cast<int>(m_devices.size()))
   {
      if (m_devices[index].type == DeviceType::PacLED64)
      {
         return m_devices[index].deviceId;
      }
   }
   return -1;
}

PacDriveSingleton::DeviceType PacDriveSingleton::GetDeviceType(int index)
{
   if (index >= 0 && index < static_cast<int>(m_devices.size()))
   {
      return m_devices[index].type;
   }
   return DeviceType::Unknown;
}

std::string PacDriveSingleton::GetDevicePath(int index)
{
   if (index >= 0 && index < static_cast<int>(m_devices.size()))
   {
      return m_devices[index].path;
   }
   return "";
}

bool PacDriveSingleton::IsPacDriveDevice(uint16_t vendorId, uint16_t productId) { return (vendorId == 0xD209) && (productId == 0x1500); }

int PacDriveSingleton::PacDriveGetIndex()
{
   for (size_t i = 0; i < m_devices.size(); i++)
   {
      if (m_devices[i].type == DeviceType::PacDrive)
      {
         return static_cast<int>(i);
      }
   }
   return -1;
}

bool PacDriveSingleton::PacDriveUHIDSetLEDStates(int index, uint16_t data)
{
   if (index < 0 || index >= static_cast<int>(m_devices.size()))
      return false;

   const DeviceInfo& device = m_devices[index];
   if (device.type != DeviceType::PacDrive)
      return false;

   libusb_device_handle* usbDevice = GetUsbDevice(index);
   if (!usbDevice)
      return false;

   uint8_t message[4] = { 0, 0, 0, 0 };
   message[2] = static_cast<uint8_t>((data >> 8) & 0xFF);
   message[3] = static_cast<uint8_t>(data & 0xFF);

   int result = libusb_control_transfer(usbDevice, 0x21, 9, 0x0200, 0, message, 4, 2000);

   return (result == 4);
}

int PacDriveSingleton::PacUIOGetIndexForDeviceId(int id)
{
   for (size_t i = 0; i < m_devices.size(); i++)
   {
      if (m_devices[i].type == DeviceType::IPACIO && m_devices[i].deviceId == id)
      {
         return static_cast<int>(i);
      }
   }
   return -1;
}

bool PacDriveSingleton::IsPacUIODevice(uint16_t vendorId, uint16_t productId) { return (vendorId == 0xD209) && (productId >= 0x0410) && (productId <= 0x0411); }

int PacDriveSingleton::ExtractPacUIOId(uint16_t productId)
{
   if (productId >= 0x0410 && productId <= 0x0411)
   {
      return static_cast<int>(productId - 0x0410);
   }
   return -1;
}

int PacDriveSingleton::PacUIOGetDeviceId(int index)
{
   if (index >= 0 && index < static_cast<int>(m_devices.size()))
   {
      if (m_devices[index].type == DeviceType::IPACIO)
      {
         return m_devices[index].deviceId;
      }
   }
   return -1;
}

std::vector<int> PacDriveSingleton::PacUIOGetIdList()
{
   std::vector<int> idList;

   EnumerateDevices();

   for (const auto& device : m_devices)
   {
      if (device.type == DeviceType::IPACIO && device.deviceId >= 0)
      {
         idList.push_back(device.deviceId);
      }
   }

   return idList;
}

bool PacDriveSingleton::PacLed64SetLEDIntensities(int index, const uint8_t* data)
{
   if (index < 0 || index >= static_cast<int>(m_devices.size()))
      return false;

   const DeviceInfo& device = m_devices[index];
   if (device.type != DeviceType::PacLED64 && device.type != DeviceType::IPACIO)
      return false;

   libusb_device_handle* usbDevice = GetUsbDevice(index);
   if (!usbDevice)
      return false;

   bool success = true;
   int maxLeds = (device.type == DeviceType::PacLED64) ? 64 : 96;

   for (int i = 0; i < maxLeds; i++)
   {
      uint8_t ledData[2];
      ledData[0] = static_cast<uint8_t>(i);
      ledData[1] = data[i];

      int result = libusb_control_transfer(usbDevice, 0x21, 9, 0x0200, 0, ledData, 2, 2000);
      if (result < 0)
      {
         success = false;
      }
   }

   return success;
}

bool PacDriveSingleton::PacLed64SetLEDIntensity(int index, int port, uint8_t intensity)
{
   if (index < 0 || index >= static_cast<int>(m_devices.size()))
      return false;

   const DeviceInfo& device = m_devices[index];
   if (device.type != DeviceType::PacLED64 && device.type != DeviceType::IPACIO)
      return false;

   int maxLeds = (device.type == DeviceType::PacLED64) ? 64 : 96;
   if (port < 0 || port >= maxLeds)
      return false;

   libusb_device_handle* usbDevice = GetUsbDevice(index);
   if (!usbDevice)
      return false;

   uint8_t data[2];
   data[0] = static_cast<uint8_t>(port);
   data[1] = intensity;

   int result = libusb_control_transfer(usbDevice, 0x21, 9, 0x0200, 0, data, 2, 2000);

   return (result >= 0);
}

bool PacDriveSingleton::PacLed64SetLEDStates(int index, int group, uint8_t data)
{
   if (index < 0 || index >= static_cast<int>(m_devices.size()))
      return false;

   const DeviceInfo& device = m_devices[index];
   if (device.type != DeviceType::PacLED64 && device.type != DeviceType::IPACIO)
      return false;

   libusb_device_handle* usbDevice = GetUsbDevice(index);
   if (!usbDevice)
      return false;

   bool success = true;

   // Special case: group 0 means turn off all LEDs
   if (group == 0)
   {
      int maxLeds = (device.type == DeviceType::PacLED64) ? 64 : 96;
      for (int ledIndex = 0; ledIndex < maxLeds; ledIndex++)
      {
         uint8_t ledData[2];
         ledData[0] = static_cast<uint8_t>(ledIndex);
         ledData[1] = 0; // intensity = 0

         int result = libusb_control_transfer(usbDevice, 0x21, 9, 0x0200, 0, ledData, 2, 2000);
         if (result < 0)
         {
            success = false;
         }
      }
   }
   else
   {
      for (int bit = 0; bit < 8; bit++)
      {
         int ledIndex = (group - 1) * 8 + bit;
         int maxLeds = (device.type == DeviceType::PacLED64) ? 64 : 96;

         if (ledIndex < maxLeds)
         {
            uint8_t intensity = (data & (1 << bit)) ? 255 : 0;
            uint8_t ledData[2];
            ledData[0] = static_cast<uint8_t>(ledIndex);
            ledData[1] = intensity;

            int result = libusb_control_transfer(usbDevice, 0x21, 9, 0x0200, 0, ledData, 2, 2000);
            if (result < 0)
            {
               success = false;
            }
         }
      }
   }

   return success;
}

bool PacDriveSingleton::PacLed64SetLEDFadeTime(int index, uint8_t fadeTime)
{
   if (index < 0 || index >= static_cast<int>(m_devices.size()))
      return false;

   const DeviceInfo& device = m_devices[index];
   if (device.type != DeviceType::PacLED64 && device.type != DeviceType::IPACIO)
      return false;

   libusb_device_handle* usbDevice = GetUsbDevice(index);
   if (!usbDevice)
      return false;

   uint8_t data[2];
   data[0] = 0x40; // Fade command base
   data[1] = fadeTime + 4;

   int result = libusb_control_transfer(usbDevice, 0x21, 9, 0x0300, 0, data, 2, 2000);

   return (result >= 0);
}

hid_device* PacDriveSingleton::GetHidDevice(int index)
{
   std::lock_guard<std::mutex> lock(m_hidDevicesMutex);

   auto it = m_hidDevices.find(index);
   if (it != m_hidDevices.end())
   {
      return it->second;
   }

   OpenHidDevice(index);

   it = m_hidDevices.find(index);
   if (it != m_hidDevices.end())
   {
      return it->second;
   }

   return nullptr;
}

void PacDriveSingleton::OpenHidDevice(int index)
{
   if (index < 0 || index >= static_cast<int>(m_devices.size()))
      return;

   const DeviceInfo& device = m_devices[index];

   if (m_hidDevices.find(index) != m_hidDevices.end())
      return;

   hid_device* hidDevice = hid_open_path(device.path.c_str());
   if (hidDevice)
   {
      m_hidDevices[index] = hidDevice;
   }
}

void PacDriveSingleton::CloseHidDevice(int index)
{
   std::lock_guard<std::mutex> lock(m_hidDevicesMutex);

   auto it = m_hidDevices.find(index);
   if (it != m_hidDevices.end())
   {
      if (it->second)
      {
         hid_close(it->second);
      }
      m_hidDevices.erase(it);
   }
}

libusb_device_handle* PacDriveSingleton::GetUsbDevice(int index)
{
   std::lock_guard<std::mutex> lock(m_usbDevicesMutex);

   auto it = m_usbDevices.find(index);
   if (it != m_usbDevices.end())
   {
      return it->second;
   }

   OpenUsbDevice(index);

   it = m_usbDevices.find(index);
   if (it != m_usbDevices.end())
   {
      return it->second;
   }

   return nullptr;
}

void PacDriveSingleton::OpenUsbDevice(int index)
{
   if (index < 0 || index >= static_cast<int>(m_devices.size()))
      return;

   const DeviceInfo& device = m_devices[index];

   if (m_usbDevices.find(index) != m_usbDevices.end())
      return;

   libusb_device_handle* handle = libusb_open_device_with_vid_pid(m_libusbContext, device.vendorId, device.productId);
   if (handle)
   {
      if (libusb_kernel_driver_active(handle, 0) == 1)
      {
         libusb_detach_kernel_driver(handle, 0);
      }

      int result = libusb_claim_interface(handle, 0);
      if (result == 0)
      {
         m_usbDevices[index] = handle;
      }
      else
      {
         libusb_close(handle);
      }
   }
}

void PacDriveSingleton::CloseUsbDevice(int index)
{
   std::lock_guard<std::mutex> lock(m_usbDevicesMutex);

   auto it = m_usbDevices.find(index);
   if (it != m_usbDevices.end())
   {
      if (it->second)
      {
         libusb_release_interface(it->second, 0);
         libusb_attach_kernel_driver(it->second, 0);
         libusb_close(it->second);
      }
      m_usbDevices.erase(it);
   }
}

}