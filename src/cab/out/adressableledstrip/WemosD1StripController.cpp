#include "WemosD1StripController.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include <algorithm>
#include <thread>
#include <chrono>
#include <climits>

namespace DOF
{

WemosD1MPStripController::WemosD1MPStripController()
   : m_sendPerLedstripLength(false)
   , m_useCompression(false)
   , m_testOnConnect(false)
{
}

void WemosD1MPStripController::SetupController()
{
   std::vector<uint8_t> receiveData;
   int bytesRead = -1;
   std::vector<uint8_t> commandData;

   TeensyStripController::SetupController();

   if (m_sendPerLedstripLength)
   {
      for (int numled = 0; numled < 10; ++numled)
      {
         int nbleds = GetNumberOfLedsPerStrip()[numled];
         if (nbleds > 0)
         {
            commandData = { (uint8_t)'Z', (uint8_t)numled, (uint8_t)(10 - 1), (uint8_t)(nbleds >> 8), (uint8_t)(nbleds & 255) };

            Log::Write(StringExtensions::Build("Resize ledstrip {0} to {1} leds.", std::to_string(numled), std::to_string(nbleds)));

            enum sp_return result = sp_blocking_write(GetComPort(), commandData.data(), 5, GetComPortTimeOutMs());
            if (result < 0)
               throw std::runtime_error(StringExtensions::Build("Failed to write ledstrip resize command: {0}", sp_last_error_message()));

            receiveData.resize(1);
            bytesRead = -1;

            try
            {
               bytesRead = ReadPortWait(receiveData.data(), 0, 1);
            }
            catch (const std::exception& e)
            {
               throw std::runtime_error(StringExtensions::Build(
                  "Expected 1 bytes after setting the number of leds for ledstrip {0} , but the read operation resulted in a exception. Will not send data to the controller.",
                  std::to_string(numled)));
            }

            if (bytesRead != 1 || receiveData[0] != (uint8_t)'A')
            {
               char receivedChar = (char)receiveData[0];
               throw std::runtime_error(StringExtensions::Build(
                  "Expected a Ack (A) after setting the number of leds for ledstrip {0}, but received no answer or a unexpected answer ({1}). Will not send data to the controller.",
                  std::to_string(numled), std::string(1, receivedChar)));
            }
         }
      }
   }

   if (m_testOnConnect)
   {
      commandData = { (uint8_t)'T' };
      Log::Write("Send a test request to the controller");

      enum sp_return result = sp_blocking_write(GetComPort(), commandData.data(), 1, GetComPortTimeOutMs());
      if (result < 0)
         throw std::runtime_error(StringExtensions::Build("Failed to write test command: {0}", sp_last_error_message()));

      std::this_thread::sleep_for(std::chrono::milliseconds(2000));

      receiveData.resize(1);
      bytesRead = -1;

      try
      {
         bytesRead = ReadPortWait(receiveData.data(), 0, 1);
      }
      catch (const std::exception& e)
      {
         throw std::runtime_error("Expected 1 bytes after requesting a test sequence, but the read operation resulted in a exception. Will not send data to the controller.");
      }

      if (bytesRead != 1 || receiveData[0] != (uint8_t)'A')
      {
         char receivedChar = (char)receiveData[0];
         throw std::runtime_error(StringExtensions::Build(
            "Expected a Ack (A) after requesting a test sequence, but received no answer or a unexpected answer ({0}). Will not send data to the controller.", std::string(1, receivedChar)));
      }

      m_testOnConnect = false;
   }
}

void WemosD1MPStripController::SendLedstripData(const std::vector<uint8_t>& outputValues, int targetPosition)
{
   if (m_useCompression)
   {
      m_compressedData.clear();
      m_uncompressedData.clear();
      m_uncompressedData.assign(outputValues.begin(), outputValues.end());

      while (!m_uncompressedData.empty())
      {
         if (m_uncompressedData.size() == 3)
         {
            m_compressedData.push_back(1);
            m_compressedData.push_back(m_uncompressedData[0]);
            m_compressedData.push_back(m_uncompressedData[1]);
            m_compressedData.push_back(m_uncompressedData[2]);
            m_uncompressedData.erase(m_uncompressedData.begin(), m_uncompressedData.begin() + 3);
         }
         else
         {
            uint8_t r = m_uncompressedData[0];
            uint8_t g = m_uncompressedData[1];
            uint8_t b = m_uncompressedData[2];
            m_uncompressedData.erase(m_uncompressedData.begin(), m_uncompressedData.begin() + 3);

            int value = (r << 16) | (g << 8) | b;
            int cnt = 1;

            while (!m_uncompressedData.empty() && ((m_uncompressedData[0] << 16) | (m_uncompressedData[1] << 8) | m_uncompressedData[2]) == value && cnt < UCHAR_MAX - 1)
            {
               m_uncompressedData.erase(m_uncompressedData.begin(), m_uncompressedData.begin() + 3);
               cnt++;
            }

            m_compressedData.push_back((uint8_t)cnt);
            m_compressedData.push_back(r);
            m_compressedData.push_back(g);
            m_compressedData.push_back(b);
         }
      }

      if (m_compressedData.size() < outputValues.size())
      {
         int nbData = m_compressedData.size() / 4;
         int nbLeds = outputValues.size() / 3;
         std::vector<uint8_t> commandData = { (uint8_t)'Q', (uint8_t)(targetPosition >> 8), (uint8_t)(targetPosition & 255), (uint8_t)(nbData >> 8), (uint8_t)(nbData & 255),
            (uint8_t)(nbLeds >> 8), (uint8_t)(nbLeds & 255) };

         enum sp_return result = sp_blocking_write(GetComPort(), commandData.data(), 7, GetComPortTimeOutMs());
         if (result < 0)
            throw std::runtime_error(StringExtensions::Build("Failed to write compressed command data: {0}", sp_last_error_message()));

         result = sp_blocking_write(GetComPort(), m_compressedData.data(), m_compressedData.size(), GetComPortTimeOutMs());
         if (result < 0)
            throw std::runtime_error(StringExtensions::Build("Failed to write compressed output values: {0}", sp_last_error_message()));
      }
      else
      {
         TeensyStripController::SendLedstripData(outputValues, targetPosition);
      }
   }
   else
   {
      TeensyStripController::SendLedstripData(outputValues, targetPosition);
   }
}

}