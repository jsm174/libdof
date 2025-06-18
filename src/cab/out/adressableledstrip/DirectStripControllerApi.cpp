#include "DirectStripControllerApi.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"

namespace DOF
{

const std::vector<std::string> DirectStripControllerApi::s_controllerNameBase = { "WS2811 Strip Controller", "Direct Strip Controller" };

DirectStripControllerApi::DirectStripControllerApi()
   : m_controllerNumber(-1)
   , m_errorCorrectionCnt(0)
   , m_ft245r(nullptr)
{
}

DirectStripControllerApi::DirectStripControllerApi(int controllerNumber)
   : m_controllerNumber(-1)
   , m_errorCorrectionCnt(0)
   , m_ft245r(nullptr)
{
   Open(controllerNumber);
}

DirectStripControllerApi::~DirectStripControllerApi() { Close(); }

void DirectStripControllerApi::ClearData()
{
   std::lock_guard<std::mutex> lock(m_ftdiLocker);

   if (m_ft245r != nullptr)
   {
      Log::Warning("DirectStripControllerApi::ClearData() - FTDI functionality not yet implemented");
   }
}

void DirectStripControllerApi::DisplayData(int length)
{
   std::lock_guard<std::mutex> lock(m_ftdiLocker);

   if (m_ft245r != nullptr)
   {
      Log::Warning("DirectStripControllerApi::DisplayData() - FTDI functionality not yet implemented");
   }
}

void DirectStripControllerApi::SetAndDisplayData(const std::vector<uint8_t>& data)
{
   std::lock_guard<std::mutex> lock(m_ftdiLocker);
   SetData(data);
   DisplayData(data.size());
}

void DirectStripControllerApi::SetData(const std::vector<uint8_t>& data)
{
   std::lock_guard<std::mutex> lock(m_ftdiLocker);

   if (m_ft245r != nullptr)
   {
      Log::Warning("DirectStripControllerApi::SetData() - FTDI functionality not yet implemented");
   }
}

std::vector<uint8_t> DirectStripControllerApi::ReadData(int length)
{
   std::lock_guard<std::mutex> lock(m_ftdiLocker);

   if (m_ft245r != nullptr)
   {
      Log::Warning("DirectStripControllerApi::ReadData() - FTDI functionality not yet implemented");
   }

   return std::vector<uint8_t>(length, 0);
}

void DirectStripControllerApi::SetAndDisplayPackedData(const std::vector<uint8_t>& data)
{
   std::lock_guard<std::mutex> lock(m_ftdiLocker);
   SetPackedData(data);
   DisplayData(data.size());
}

void DirectStripControllerApi::SetPackedData(const std::vector<uint8_t>& data)
{
   std::lock_guard<std::mutex> lock(m_ftdiLocker);

   if (m_ft245r != nullptr)
   {
      Log::Warning("DirectStripControllerApi::SetPackedData() - FTDI functionality not yet implemented");
   }
}

bool DirectStripControllerApi::GetDeviceIsPresent() const { return false; }

void DirectStripControllerApi::Open(int controllerNumber)
{
   std::lock_guard<std::mutex> lock(m_ftdiLocker);

   Close();
   m_controllerNumber = controllerNumber;

   Log::Warning(StringExtensions::Build("DirectStripControllerApi::Open({0}) - FTDI functionality not yet implemented", std::to_string(controllerNumber)));
}

void DirectStripControllerApi::Close()
{
   std::lock_guard<std::mutex> lock(m_ftdiLocker);

   if (m_ft245r != nullptr)
   {
      Log::Warning("DirectStripControllerApi::Close() - FTDI functionality not yet implemented");
      m_ft245r = nullptr;
   }
}

std::vector<int> DirectStripControllerApi::GetAvailableControllerNumbers()
{
   Log::Warning("DirectStripControllerApi::GetAvailableControllerNumbers() - FTDI functionality not yet implemented");
   return std::vector<int>();
}

}