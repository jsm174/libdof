#pragma once

#include "DOF/DOF.h"
#include "OutputControllerCompleteBase.h"

namespace DOF
{

class OutputControllerFlexCompleteBase : public OutputControllerCompleteBase
{
public:
   OutputControllerFlexCompleteBase();
   ~OutputControllerFlexCompleteBase();

   virtual int GetNumberOfOutputs() { return m_numberOfOutputs; }
   virtual void SetNumberOfOutputs(int numberOfOutputs);

   int GetNumberOfConfiguredOutputs() override;

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;

private:
   int m_numberOfOutputs;
};

}
