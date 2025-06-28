#pragma once

#include "../OutputControllerCompleteBase.h"
#include <string>

namespace DOF
{

class Engine;

class ArtNet : public OutputControllerCompleteBase
{
public:
   ArtNet();
   virtual ~ArtNet();

   short GetUniverse() const { return m_universe; }
   void SetUniverse(short value) { m_universe = value; }

   const std::string& GetBroadcastAddress() const { return m_broadcastAddress; }
   void SetBroadcastAddress(const std::string& value) { m_broadcastAddress = value; }

   virtual std::string GetXmlElementName() const override { return "ArtNet"; }
   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;

protected:
   virtual int GetNumberOfConfiguredOutputs() override;
   virtual bool VerifySettings() override;
   virtual void UpdateOutputs(const std::vector<uint8_t>& outputValues) override;
   virtual void ConnectToController() override;
   virtual void DisconnectFromController() override;

private:
   Engine* m_engine;
   short m_universe;
   std::string m_broadcastAddress;

   static const int DMX_CHANNELS = 512;
};

}