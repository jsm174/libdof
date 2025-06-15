#include "OutputControllerList.h"

#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include "NullOutputController.h"

#ifdef __LIBSERIALPORT__
#include "lw/LedWiz.h"
#endif

#ifdef __HIDAPI__
#include "ps/Pinscape.h"
#endif

namespace DOF
{

OutputControllerList::OutputControllerList() { }

OutputControllerList::~OutputControllerList() { Clear(); }

void OutputControllerList::Clear()
{
   for (IOutputController* controller : *this)
      delete controller;
   clear();
}

void OutputControllerList::Init(Cabinet* pCabinet)
{
   Log::Write("Initializing output controllers");

   for (IOutputController* pController : *this)
      pController->Init(pCabinet);

   Log::Write("Output controllers initialized");
}

void OutputControllerList::Finish()
{
   Log::Write("Finishing output controllers");

   for (IOutputController* pController : *this)
      pController->Finish();

   Log::Write("Output controllers finished");
}

void OutputControllerList::Update()
{
   for (IOutputController* pController : *this)
      pController->Update();
}

bool OutputControllerList::Contains(const std::string& name) const
{
   for (IOutputController* pController : *this)
   {
      if (pController->GetName() == name)
      {
         return true;
      }
   }
   return false;
}

IOutputController* OutputControllerList::CreateController(const std::string& typeName)
{
   if (typeName == "NullOutputController")
      return new NullOutputController();
#ifdef __HIDAPI__
   else if (typeName == "Pinscape")
      return new Pinscape();
#endif
#ifdef __LIBSERIALPORT__
   else if (typeName == "LedWiz")
      return new LedWiz();
#endif
   else
   {
      Log::Warning(StringExtensions::Build("Unknown output controller type: {0}", typeName));
      return nullptr;
   }
}

XMLElement* OutputControllerList::ToXml(XMLDocument& doc) const
{
   XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   for (const IOutputController* controller : *this)
   {
      if (controller)
      {
         XMLElement* controllerElement = controller->ToXml(doc);
         if (controllerElement)
            element->InsertEndChild(controllerElement);
      }
   }

   return element;
}

bool OutputControllerList::FromXml(const XMLElement* element)
{
   if (!element)
      return false;

   Clear();

   const XMLElement* controllerElement = element->FirstChildElement();
   while (controllerElement)
   {
      std::string typeName = controllerElement->Name();
      IOutputController* controller = CreateController(typeName);

      if (controller)
      {
         if (controller->FromXml(controllerElement))
         {
            push_back(controller);
            Log::Write(StringExtensions::Build("Loaded output controller: {0} ({1})", controller->GetName(), typeName));
         }
         else
         {
            Log::Warning(StringExtensions::Build("Failed to load output controller from XML: {0}", typeName));
            delete controller;
         }
      }

      controllerElement = controllerElement->NextSiblingElement();
   }

   return true;
}

}
