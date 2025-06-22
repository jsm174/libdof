#pragma once

#include <string>
#include <memory>
#include <functional>
#include <ftdi.h>

namespace DOF
{

class FTDI
{
public:
   enum FT_STATUS
   {
      FT_OK = 0,
      FT_INVALID_HANDLE,
      FT_DEVICE_NOT_FOUND,
      FT_DEVICE_NOT_OPENED,
      FT_IO_ERROR,
      FT_INSUFFICIENT_RESOURCES,
      FT_INVALID_PARAMETER,
      FT_INVALID_BAUD_RATE,
      FT_DEVICE_NOT_OPENED_FOR_ERASE,
      FT_DEVICE_NOT_OPENED_FOR_WRITE,
      FT_FAILED_TO_WRITE_DEVICE,
      FT_EEPROM_READ_FAILED,
      FT_EEPROM_WRITE_FAILED,
      FT_EEPROM_ERASE_FAILED,
      FT_EEPROM_NOT_PRESENT,
      FT_EEPROM_NOT_PROGRAMMED,
      FT_INVALID_ARGS,
      FT_NOT_SUPPORTED,
      FT_OTHER_ERROR
   };

   enum FT_DEVICE_TYPE
   {
      FT_DEVICE_BM,
      FT_DEVICE_AM,
      FT_DEVICE_100AX,
      FT_DEVICE_UNKNOWN,
      FT_DEVICE_2232C,
      FT_DEVICE_232R,
      FT_DEVICE_2232H,
      FT_DEVICE_4232H,
      FT_DEVICE_232H,
      FT_DEVICE_X_SERIES,
      FT_DEVICE_4222H_0,
      FT_DEVICE_4222H_1_2,
      FT_DEVICE_4222H_3,
      FT_DEVICE_4222_PROG
   };

   enum FT_OPEN_BY
   {
      FT_OPEN_BY_SERIAL_NUMBER = 1,
      FT_OPEN_BY_DESCRIPTION = 2
   };

   struct FT_DEVICE_INFO_NODE
   {
      uint32_t Flags;
      uint32_t Type;
      uint32_t ID;
      uint32_t LocId;
      std::string SerialNumber;
      std::string Description;
      void* ftHandle;
   };

   FTDI();
   virtual ~FTDI();

   FT_STATUS GetNumberOfDevices(uint32_t& devcount);
   FT_STATUS GetDeviceInfoList(FT_DEVICE_INFO_NODE* devinfo, uint32_t& numdevs);
   FT_STATUS OpenEx(const std::string& identifier, FT_OPEN_BY dwFlags);
   FT_STATUS Close();
   FT_STATUS Read(uint8_t* ftBuffer, uint32_t dwBytesToRead, uint32_t& dwBytesReturned);
   FT_STATUS Write(const uint8_t* ftBuffer, uint32_t dwBytesToWrite, uint32_t& dwBytesWritten);
   FT_STATUS SetBitMode(uint8_t ucMask, uint8_t ucEnable);
   FT_STATUS GetBitMode(uint8_t& ucMode);
   FT_STATUS SetBaudRate(uint32_t dwBaudRate);
   FT_STATUS SetTimeouts(uint32_t dwReadTimeout, uint32_t dwWriteTimeout);
   FT_STATUS SetLatencyTimer(uint8_t ucLatency);
   FT_STATUS GetLatencyTimer(uint8_t& ucLatency);
   FT_STATUS Purge(uint32_t dwMask);

   bool IsOpen() const { return m_ftdiContext != nullptr && m_isOpen; }

private:
   struct ftdi_context* m_ftdiContext;
   bool m_isOpen;

   FT_STATUS ConvertLibftdiError(int error);
};

}