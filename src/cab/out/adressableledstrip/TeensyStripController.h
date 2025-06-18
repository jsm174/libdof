#pragma once

#include "DOF/DOF.h"
#include "../OutputControllerCompleteBase.h"
#include <libserialport.h>
#include <string>
#include <vector>

namespace DOF
{

enum class Parity
{
   None,
   Odd,
   Even,
   Mark,
   Space
};

enum class StopBits
{
   None,
   One,
   Two,
   OnePointFive
};

class TeensyStripController : public OutputControllerCompleteBase
{
protected:
   std::vector<int> m_numberOfLedsPerStrip;

public:
   TeensyStripController();
   virtual ~TeensyStripController();

   int GetNumberOfLedsStrip1() const { return m_numberOfLedsPerStrip[0]; }
   void SetNumberOfLedsStrip1(int value);

   int GetNumberOfLedsStrip2() const { return m_numberOfLedsPerStrip[1]; }
   void SetNumberOfLedsStrip2(int value);

   int GetNumberOfLedsStrip3() const { return m_numberOfLedsPerStrip[2]; }
   void SetNumberOfLedsStrip3(int value);

   int GetNumberOfLedsStrip4() const { return m_numberOfLedsPerStrip[3]; }
   void SetNumberOfLedsStrip4(int value);

   int GetNumberOfLedsStrip5() const { return m_numberOfLedsPerStrip[4]; }
   void SetNumberOfLedsStrip5(int value);

   int GetNumberOfLedsStrip6() const { return m_numberOfLedsPerStrip[5]; }
   void SetNumberOfLedsStrip6(int value);

   int GetNumberOfLedsStrip7() const { return m_numberOfLedsPerStrip[6]; }
   void SetNumberOfLedsStrip7(int value);

   int GetNumberOfLedsStrip8() const { return m_numberOfLedsPerStrip[7]; }
   void SetNumberOfLedsStrip8(int value);

   int GetNumberOfLedsStrip9() const { return m_numberOfLedsPerStrip[8]; }
   void SetNumberOfLedsStrip9(int value);

   int GetNumberOfLedsStrip10() const { return m_numberOfLedsPerStrip[9]; }
   void SetNumberOfLedsStrip10(int value);

   std::string GetComPortName() const { return m_comPortName; }
   void SetComPortName(const std::string& value) { m_comPortName = value; }

   int GetComPortBaudRate() const { return m_comPortBaudRate; }
   void SetComPortBaudRate(int value) { m_comPortBaudRate = value; }

   Parity GetComPortParity() const { return m_comPortParity; }
   void SetComPortParity(Parity value) { m_comPortParity = value; }

   int GetComPortDataBits() const { return m_comPortDataBits; }
   void SetComPortDataBits(int value) { m_comPortDataBits = value; }

   StopBits GetComPortStopBits() const { return m_comPortStopBits; }
   void SetComPortStopBits(StopBits value) { m_comPortStopBits = value; }

   int GetComPortTimeOutMs() const { return m_comPortTimeOutMs; }
   void SetComPortTimeOutMs(int value);

   int GetComPortOpenWaitMs() const { return m_comPortOpenWaitMs; }
   void SetComPortOpenWaitMs(int value);

   int GetComPortHandshakeStartWaitMs() const { return m_comPortHandshakeStartWaitMs; }
   void SetComPortHandshakeStartWaitMs(int value);

   int GetComPortHandshakeEndWaitMs() const { return m_comPortHandshakeEndWaitMs; }
   void SetComPortHandshakeEndWaitMs(int value);

   bool GetComPortDtrEnable() const { return m_comPortDtrEnable; }
   void SetComPortDtrEnable(bool value) { m_comPortDtrEnable = value; }

   virtual std::string GetXmlElementName() const override { return "TeensyStripController"; }

   virtual bool FromXml(const tinyxml2::XMLElement* element) override;

protected:
   virtual int GetNumberOfConfiguredOutputs() override;
   virtual bool VerifySettings() override;
   virtual void ConnectToController() override;
   virtual void DisconnectFromController() override;
   virtual void UpdateOutputs(const std::vector<uint8_t>& outputValues) override;

   virtual void SendLedstripData(const std::vector<uint8_t>& outputValues, int targetPosition);
   virtual void SetupController();
   int ReadPortWait(uint8_t* buffer, int bufferOffset, int numberOfBytes);

   const std::vector<int>& GetNumberOfLedsPerStrip() const { return m_numberOfLedsPerStrip; }
   struct sp_port* GetComPort() const { return m_comPort; }

private:
   std::string m_comPortName;
   int m_comPortBaudRate;
   Parity m_comPortParity;
   int m_comPortDataBits;
   StopBits m_comPortStopBits;
   int m_comPortTimeOutMs;
   int m_comPortOpenWaitMs;
   int m_comPortHandshakeStartWaitMs;
   int m_comPortHandshakeEndWaitMs;
   bool m_comPortDtrEnable;

   struct sp_port* m_comPort;
   int m_numberOfLedsPerChannel;
};

}