#include "NullOutputController.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"

namespace DOF
{

NullOutputController::NullOutputController()
   : m_numberOfOutputs(32)
   , m_logOutputChanges(false)
{
   SetName("NullOutputController");
}

NullOutputController::NullOutputController(int numberOfOutputs)
   : m_numberOfOutputs(numberOfOutputs)
   , m_logOutputChanges(false)
{
   SetName("NullOutputController");
}

void NullOutputController::SetNumberOfOutputs(int numberOfOutputs)
{
   if (numberOfOutputs < 1)
      numberOfOutputs = 1;
   if (numberOfOutputs > 256)
      numberOfOutputs = 256;

   m_numberOfOutputs = numberOfOutputs;
   SetupOutputs();
}

void NullOutputController::Init(Cabinet* cabinet)
{
   OutputControllerCompleteBase::Init(cabinet);
   SetupOutputs();
   Log::Write(StringExtensions::Build("NullOutputController initialized with {0} outputs", std::to_string(m_numberOfOutputs)));
}

void NullOutputController::Finish()
{
   Log::Write("NullOutputController finished");
   OutputControllerCompleteBase::Finish();
}

void NullOutputController::Update() { OutputControllerCompleteBase::Update(); }

tinyxml2::XMLElement* NullOutputController::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   if (!GetName().empty())
   {
      tinyxml2::XMLElement* nameElement = doc.NewElement("Name");
      nameElement->SetText(GetName().c_str());
      element->InsertEndChild(nameElement);
   }

   return element;
}

bool NullOutputController::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   const char* name = element->Attribute("Name");
   if (name)
      SetName(name);

   return true;
}

bool NullOutputController::VerifySettings() { return m_numberOfOutputs > 0 && m_numberOfOutputs <= 256; }

void NullOutputController::ConnectToController() { Log::Write("NullOutputController: Connected (no hardware)"); }

void NullOutputController::DisconnectFromController() { Log::Write("NullOutputController: Disconnected (no hardware)"); }

void NullOutputController::UpdateOutputs(const std::vector<uint8_t>& outputValues)
{
   if (m_logOutputChanges && !outputValues.empty())
   {
      Log::Write(StringExtensions::Build("NullOutputController: Processing {0} output values", std::to_string(outputValues.size())));

      for (size_t i = 0; i < outputValues.size(); ++i)
      {
         if (outputValues[i] > 0)
         {
            Log::Write(StringExtensions::Build("  Output {0}: {1}", std::to_string(i + 1), std::to_string(outputValues[i])));
         }
      }
   }
}

}