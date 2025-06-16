#include "Cabinet.h"

#include "../Log.h"
#include "../general/StringExtensions.h"
#include "../general/FileInfo.h"
#include "../general/FileReader.h"
#include "CabinetOutputList.h"
#include "out/IAutoConfigOutputController.h"
#include "out/OutputControllerList.h"
#include "toys/ToyList.h"
#include "schedules/ScheduledSettings.h"
#include "sequencer/SequentialOutputSettings.h"
#include <sstream>

#ifdef __HIDAPI__
#include <hidapi/hidapi.h>
#include "out/ps/Pinscape.h"
#include "out/ps/PinscapeAutoConfigurator.h"
#include "out/pspico/PinscapePico.h"
#include "out/pspico/PinscapePicoAutoConfigurator.h"
#include "out/lw/LedWizAutoConfigurator.h"
#endif

namespace DOF
{

Cabinet::Cabinet()
{
   m_autoConfigEnabled = true;
   m_pOutputControllers = new OutputControllerList();
   m_pOutputs = new CabinetOutputList(this);
   m_pToys = new ToyList();

#ifdef __HIDAPI__
   hid_init();

   Pinscape::Initialize();
   PinscapePico::Initialize();
#endif
}

Cabinet::~Cabinet()
{
#ifdef __HIDAPI__
   hid_exit();
#endif
}

void Cabinet::AutoConfig()
{
   Log::Write("Cabinet auto configuration started");

   std::vector<IAutoConfigOutputController*> items;

#ifdef __HIDAPI__
   items.push_back(new PinscapeAutoConfigurator());
   items.push_back(new PinscapePicoAutoConfigurator());
   items.push_back(new LedWizAutoConfigurator());
#endif

   for (auto& item : items)
   {
      item->AutoConfig(this);
   }

   Log::Write("Cabinet auto configuration finished");
}

std::string Cabinet::GetConfigXml()
{
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* root = ToXml(doc);
   if (!root)
      return "";

   doc.InsertFirstChild(root);

   tinyxml2::XMLPrinter printer;
   doc.Print(&printer);
   return printer.CStr();
}

void Cabinet::SaveConfigXml(const std::string& filename)
{
   std::string xml = GetConfigXml();
   if (xml.empty())
   {
      Log::Warning("Cannot save cabinet config - XML generation failed");
      return;
   }

   tinyxml2::XMLDocument doc;
   if (doc.Parse(xml.c_str()) != tinyxml2::XML_SUCCESS)
   {
      Log::Warning("Cannot save cabinet config - XML parsing failed");
      return;
   }

   if (doc.SaveFile(filename.c_str()) != tinyxml2::XML_SUCCESS)
   {
      Log::Warning(StringExtensions::Build("Failed to save cabinet config to file: {0}", filename));
   }
   else
   {
      Log::Write(StringExtensions::Build("Cabinet config saved to: {0}", filename));
   }
}

Cabinet* Cabinet::GetCabinetFromConfigXmlFile(FileInfo* cabinetConfigFile) { return cabinetConfigFile ? GetCabinetFromConfigXmlFile(cabinetConfigFile->FullName()) : nullptr; }

Cabinet* Cabinet::GetCabinetFromConfigXmlFile(const std::string& filename)
{
   std::string xml;

   try
   {
      xml = FileReader::ReadFileToString(filename);
   }
   catch (...)
   {
      Log::Exception(StringExtensions::Build("Could not load cabinet config from {0}.", filename));
   }

   return GetCabinetFromConfigXml(xml);
}

Cabinet* Cabinet::GetCabinetFromConfigXml(const std::string& configXml)
{
   if (configXml.empty())
   {
      Log::Warning("Cannot load cabinet config from empty XML");
      return nullptr;
   }

   tinyxml2::XMLDocument doc;
   if (doc.Parse(configXml.c_str()) != tinyxml2::XML_SUCCESS)
   {
      Log::Exception(StringExtensions::Build("Could not parse cabinet config XML: {0}", doc.ErrorStr()));
      return nullptr;
   }

   tinyxml2::XMLElement* root = doc.FirstChildElement("Cabinet");
   if (!root)
   {
      Log::Exception("Cabinet config XML missing root Cabinet element");
      return nullptr;
   }

   Cabinet* cabinet = new Cabinet();
   if (!cabinet->FromXml(root))
   {
      delete cabinet;
      Log::Exception("Failed to deserialize cabinet config from XML");
      return nullptr;
   }

   return cabinet;
}

bool Cabinet::TestCabinetConfigXmlFile(const std::string& filename)
{
   try
   {
      std::string xml = FileReader::ReadFileToString(filename);
      Cabinet* testCabinet = GetCabinetFromConfigXml(xml);
      if (testCabinet)
      {
         delete testCabinet;
         return true;
      }
   }
   catch (...)
   {
      Log::Exception(StringExtensions::Build("Error testing cabinet config file: {0}", filename));
   }

   return false;
}

void Cabinet::Init(ICabinetOwner* pCabinetOwner)
{
   Log::Write("Initializing cabinet");
   m_pOwner = pCabinetOwner;
   m_pOutputControllers->Init(this);
   m_pToys->Init(this);

   m_pOutputs->ConnectOutputsToControllers();

   Log::Write("Cabinet initialized");
}

void Cabinet::Update()
{
   m_pToys->UpdateOutputs();
   m_pOutputControllers->Update();
}

void Cabinet::Finish()
{
   Log::Write("Finishing cabinet");

   m_pToys->Finish();
   m_pOutputControllers->Finish();
   Log::Write("Cabinet finished");
}

ScheduledSettings* Cabinet::GetScheduledSettings() { return &ScheduledSettings::GetInstance(); }

SequentialOutputSettings* Cabinet::GetSequentialOutputSettings() { return &SequentialOutputSettings::GetInstance(); }

tinyxml2::XMLElement* Cabinet::ToXml(tinyxml2::XMLDocument& doc) const
{
   tinyxml2::XMLElement* element = doc.NewElement(GetXmlElementName().c_str());

   if (!m_name.empty())
      element->SetAttribute("Name", m_name.c_str());

   if (!m_cabinetConfigurationFilename.empty())
      element->SetAttribute("CabinetConfigurationFilename", m_cabinetConfigurationFilename.c_str());

   element->SetAttribute("AutoConfigEnabled", m_autoConfigEnabled);

   if (m_pOutputControllers)
   {
      tinyxml2::XMLElement* controllersElement = m_pOutputControllers->ToXml(doc);
      if (controllersElement)
         element->InsertEndChild(controllersElement);
   }

   if (m_pToys)
   {
      tinyxml2::XMLElement* toysElement = doc.NewElement("Toys");

      element->InsertEndChild(toysElement);
   }

   ScheduledSettings* scheduledSettings = &ScheduledSettings::GetInstance();
   if (scheduledSettings && !scheduledSettings->empty())
   {
      tinyxml2::XMLElement* scheduledElement = scheduledSettings->ToXml(doc);
      if (scheduledElement)
         element->InsertEndChild(scheduledElement);
   }

   SequentialOutputSettings* sequentialSettings = &SequentialOutputSettings::GetInstance();
   if (sequentialSettings && !sequentialSettings->empty())
   {
      tinyxml2::XMLElement* sequentialElement = sequentialSettings->ToXml(doc);
      if (sequentialElement)
         element->InsertEndChild(sequentialElement);
   }

   return element;
}

bool Cabinet::FromXml(const tinyxml2::XMLElement* element)
{
   if (!element)
      return false;

   const char* name = element->Attribute("Name");
   if (name)
      m_name = name;

   const char* configFilename = element->Attribute("CabinetConfigurationFilename");
   if (configFilename)
      m_cabinetConfigurationFilename = configFilename;

   element->QueryBoolAttribute("AutoConfigEnabled", &m_autoConfigEnabled);

   const tinyxml2::XMLElement* scheduledElement = element->FirstChildElement("ScheduledSettings");
   if (scheduledElement)
   {
      ScheduledSettings& scheduledSettings = ScheduledSettings::GetInstance();
      scheduledSettings.FromXml(scheduledElement);
   }

   const tinyxml2::XMLElement* sequentialElement = element->FirstChildElement("SequentialOutputSettings");
   if (sequentialElement)
   {
      SequentialOutputSettings& sequentialSettings = SequentialOutputSettings::GetInstance();
      sequentialSettings.FromXml(sequentialElement);
   }

   const tinyxml2::XMLElement* controllersElement = element->FirstChildElement("OutputControllers");
   if (controllersElement && m_pOutputControllers)
   {
      m_pOutputControllers->FromXml(controllersElement);
   }

   return true;
}

}
