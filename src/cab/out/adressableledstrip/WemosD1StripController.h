#pragma once

#include "DOF/DOF.h"
#include "TeensyStripController.h"
#include <vector>
#include <list>

namespace DOF
{

class WemosD1MPStripController : public TeensyStripController
{
public:
   WemosD1MPStripController();
   virtual ~WemosD1MPStripController() = default;

   bool GetSendPerLedstripLength() const { return m_sendPerLedstripLength; }
   void SetSendPerLedstripLength(bool value) { m_sendPerLedstripLength = value; }

   bool GetUseCompression() const { return m_useCompression; }
   void SetUseCompression(bool value) { m_useCompression = value; }

   bool GetTestOnConnect() const { return m_testOnConnect; }
   void SetTestOnConnect(bool value) { m_testOnConnect = value; }

   virtual std::string GetXmlElementName() const override { return "WemosD1MPStripController"; }
   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;

protected:
   virtual void SetupController() override;
   virtual void SendLedstripData(const std::vector<uint8_t>& outputValues, int targetPosition) override;

protected:
   std::vector<uint8_t> m_compressedData;
   std::vector<uint8_t> m_uncompressedData;

private:
   bool m_sendPerLedstripLength;
   bool m_useCompression;
   bool m_testOnConnect;
};

}