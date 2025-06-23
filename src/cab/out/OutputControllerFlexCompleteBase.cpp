#include "OutputControllerFlexCompleteBase.h"
#include <tinyxml2/tinyxml2.h>

namespace DOF
{

OutputControllerFlexCompleteBase::OutputControllerFlexCompleteBase() { }

OutputControllerFlexCompleteBase::~OutputControllerFlexCompleteBase() { }

void OutputControllerFlexCompleteBase::SetNumberOfOutputs(int numberOfOutputs)
{
   m_numberOfOutputs = numberOfOutputs;
   OutputControllerCompleteBase::SetupOutputs();
}

int OutputControllerFlexCompleteBase::GetNumberOfConfiguredOutputs() { return m_numberOfOutputs; }

tinyxml2::XMLElement* OutputControllerFlexCompleteBase::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = OutputControllerCompleteBase::ToXml(doc);

   tinyxml2::XMLElement* numberOfOutputsElement = doc.NewElement("NumberOfOutputs");
   numberOfOutputsElement->SetText(m_numberOfOutputs);
   element->InsertEndChild(numberOfOutputsElement);

   return element;
}

bool OutputControllerFlexCompleteBase::FromXml(const tinyxml2::XMLElement* element)
{
   if (!OutputControllerCompleteBase::FromXml(element))
      return false;

   const tinyxml2::XMLElement* numberOfOutputsElement = element->FirstChildElement("NumberOfOutputs");
   if (numberOfOutputsElement && numberOfOutputsElement->GetText())
   {
      try
      {
         int numberOfOutputs = std::stoi(numberOfOutputsElement->GetText());
         SetNumberOfOutputs(numberOfOutputs);
      }
      catch (...)
      {
         return false;
      }
   }

   return true;
}

}
