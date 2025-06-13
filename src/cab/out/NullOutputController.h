#pragma once

#include "OutputControllerCompleteBase.h"

namespace DOF
{

class NullOutputController : public OutputControllerCompleteBase
{
public:
   NullOutputController();
   NullOutputController(int numberOfOutputs);
   virtual ~NullOutputController() = default;

   virtual void Init(Cabinet* pCabinet) override;
   virtual void Finish() override;
   virtual void Update() override;
   int GetNumberOfOutputs() const { return m_numberOfOutputs; }
   void SetNumberOfOutputs(int numberOfOutputs);
   virtual XMLElement* ToXml(XMLDocument& doc) const override;
   virtual bool FromXml(const XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "NullOutputController"; }

protected:
   virtual int GetNumberOfConfiguredOutputs() override { return m_numberOfOutputs; }
   virtual bool VerifySettings() override;
   virtual void ConnectToController() override;
   virtual void DisconnectFromController() override;
   virtual void UpdateOutputs(const std::vector<uint8_t>& outputValues) override;

private:
   int m_numberOfOutputs;
   bool m_logOutputChanges;
};

}