#include "ArtNet.h"
#include "artnetengine/Engine.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"
#include <stdexcept>

namespace DOF
{

ArtNet::ArtNet()
   : m_engine(nullptr)
   , m_universe(0)
   , m_broadcastAddress("")
{
}

ArtNet::~ArtNet() { }

int ArtNet::GetNumberOfConfiguredOutputs() { return DMX_CHANNELS; }

bool ArtNet::VerifySettings() { return true; }

void ArtNet::UpdateOutputs(const std::vector<uint8_t>& outputValues)
{
   try
   {
      if (m_engine != nullptr)
      {
         if (outputValues.size() == DMX_CHANNELS)
         {
            m_engine->SendDMX(m_broadcastAddress, m_universe, outputValues, DMX_CHANNELS);
         }
         else
         {
            std::string msg = StringExtensions::Build("{0} {1} sent the wrong number of bytes to output.", GetXmlElementName(), GetName());
            Log::Exception(msg);
            throw std::runtime_error(msg);
         }
      }
      else
      {
         std::string msg = StringExtensions::Build(
            "{0} {1} (Universe: {2}, Broadcast Address: {3}) is not connected.", { GetXmlElementName(), GetName(), std::to_string(m_universe), m_broadcastAddress });
         Log::Exception(msg);
         throw std::runtime_error(msg);
      }
   }
   catch (const std::exception& e)
   {
      std::string msg = StringExtensions::Build(
         "{0} {1} (Universe: {2}, Broadcast Address: {3}) could not send data: {4}", { GetXmlElementName(), GetName(), std::to_string(m_universe), m_broadcastAddress, e.what() });
      Log::Exception(msg);
      throw std::runtime_error(msg);
   }
}

void ArtNet::ConnectToController()
{
   if (m_engine == nullptr)
   {
      try
      {
         m_engine = Engine::GetInstance();
         std::vector<uint8_t> zeroData(DMX_CHANNELS, 0);
         m_engine->SendDMX(m_broadcastAddress, m_universe, zeroData, DMX_CHANNELS);
      }
      catch (const std::exception& e)
      {
         m_engine = nullptr;
         std::string msg = StringExtensions::Build(
            "{0} {1} (Universe: {2}, Broadcast Address: {3}) could not connect: {4}", { GetXmlElementName(), GetName(), std::to_string(m_universe), m_broadcastAddress, e.what() });
         Log::Exception(msg);
         throw std::runtime_error(msg);
      }
   }
}

void ArtNet::DisconnectFromController()
{
   try
   {
      if (m_engine != nullptr)
      {
         std::vector<uint8_t> zeroData(DMX_CHANNELS, 0);
         m_engine->SendDMX(m_broadcastAddress, m_universe, zeroData, DMX_CHANNELS);
      }
   }
   catch (...)
   {
   }

   if (m_engine != nullptr)
   {
      m_engine = nullptr;
   }
}

}