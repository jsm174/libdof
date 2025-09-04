#include "OutputControllerList.h"

#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include "NullOutputController.h"

#ifdef __HIDAPI__
#include "ps/Pinscape.h"
#include "pspico/PinscapePico.h"
#include "lw/LedWiz.h"
#include "dudescab/DudesCab.h"
#endif

#ifdef __LIBUSB__
#include "pac/PacLed64.h"
#include "pac/PacDrive.h"
#include "pac/PacUIO.h"
#endif

#ifdef __LIBFTDI__
#include "ftdichip/FT245RBitbangController.h"
#endif

#ifdef __LIBSERIALPORT__
#include "adressableledstrip/TeensyStripController.h"
#include "adressableledstrip/WemosD1StripController.h"
#include "comport/PinControl.h"
#include "pinone/PinOne.h"
#endif

#include "dmx/ArtNet.h"

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

void OutputControllerList::Init(Cabinet* cabinet)
{
   Log::Write("Initializing output controllers");

   for (IOutputController* controller : *this)
      controller->Init(cabinet);

   Log::Write("Output controllers initialized");
}

void OutputControllerList::Finish()
{
   Log::Write("Finishing output controllers");

   for (IOutputController* controller : *this)
      controller->Finish();

#ifdef __HIDAPI__
   Pinscape::ClearDevices();
   PinscapePico::ClearDevices();
   DudesCab::ClearDevices();
#endif

   Log::Write("Output controllers finished");
}

void OutputControllerList::Update()
{
   for (IOutputController* controller : *this)
      controller->Update();
}

bool OutputControllerList::Contains(const std::string& name) const
{
   for (IOutputController* controller : *this)
   {
      if (controller->GetName() == name)
      {
         return true;
      }
   }
   return false;
}

IOutputController* OutputControllerList::GetByName(const std::string& name) const
{
   for (IOutputController* controller : *this)
   {
      if (controller->GetName() == name)
      {
         return controller;
      }
   }
   return nullptr;
}

IOutputController* OutputControllerList::operator[](const std::string& name) const { return GetByName(name); }

IOutputController* OutputControllerList::CreateController(const std::string& typeName)
{
   if (typeName == "NullOutputController")
      return new NullOutputController();
#ifdef __HIDAPI__
   else if (typeName == "Pinscape")
      return new Pinscape();
   else if (typeName == "PinscapePico")
      return new PinscapePico();
   else if (typeName == "LedWiz")
      return new LedWiz();
   else if (typeName == "DudesCab")
      return new DudesCab();
#endif
#ifdef __LIBUSB__
   else if (typeName == "PacLed64")
      return new PacLed64();
   else if (typeName == "PacDrive")
      return new PacDrive();
   else if (typeName == "PacUIO")
      return new PacUIO();
#endif
#ifdef __LIBFTDI__
   else if (typeName == "FT245RBitbangController")
      return new FT245RBitbangController();
#endif
#ifdef __LIBSERIALPORT__
   else if (typeName == "TeensyStripController")
      return new TeensyStripController();
   else if (typeName == "WemosD1MPStripController")
      return new WemosD1MPStripController();
   else if (typeName == "PinControl")
      return new PinControl();
   else if (typeName == "PinOne")
      return new PinOne();
#endif
   else if (typeName == "ArtNet")
      return new ArtNet();
   else
   {
      Log::Warning(StringExtensions::Build("Unknown output controller type: {0}", typeName));
      return nullptr;
   }
}

tinyxml2::XMLElement* OutputControllerList::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   for (const IOutputController* controller : *this)
   {
      if (controller)
      {
         tinyxml2::XMLElement* controllerElement = controller->ToXml(doc);
         if (controllerElement)
            element->InsertEndChild(controllerElement);
      }
   }

   return element;
}

bool OutputControllerList::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   Clear();

   const tinyxml2::XMLElement* controllerElement = element->FirstChildElement();
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
