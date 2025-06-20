#pragma once

#include "DOF/DOF.h"
#include <vector>
#include <string>
#include <mutex>

namespace DOF
{

class DirectStripControllerApi
{
public:
   DirectStripControllerApi();
   DirectStripControllerApi(int controllerNumber);
   ~DirectStripControllerApi();

   void ClearData();
   void DisplayData(int length);
   void SetAndDisplayData(const std::vector<uint8_t>& data);
   void SetData(const std::vector<uint8_t>& data);
   std::vector<uint8_t> ReadData(int length);
   void SetAndDisplayPackedData(const std::vector<uint8_t>& data);
   void SetPackedData(const std::vector<uint8_t>& data);

   int GetControllerNumber() const { return m_controllerNumber; }
   bool GetDeviceIsPresent() const;

   void Open(int controllerNumber);
   void Close();

   static std::vector<int> GetAvailableControllerNumbers();

private:
   static const std::vector<std::string> s_controllerNameBase;

   int m_controllerNumber;
   int m_errorCorrectionCnt;
   std::mutex m_ftdiLocker;

   void* m_ft245r;
};

}