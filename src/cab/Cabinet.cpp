#include "Cabinet.h"

#include "../Log.h"
#include "../general/FileInfo.h"
#include "../general/FileReader.h"
#include "CabinetOutputList.h"
#include "out/IAutoConfigOutputController.h"
#include "out/OutputControllerList.h"
#include "toys/ToyList.h"

#ifdef __HIDAPI__
#include <hidapi/hidapi.h>

#include "out/ps/Pinscape.h"
#include "out/ps/PinscapeAutoConfigurator.h"
#endif

namespace DOF
{

Cabinet::Cabinet()
{
   m_autoConfigEnabled = true;
   m_pOutputControllers = new OutputControllerList();
   m_pOutputs = new CabinetOutputList(this);
   m_pToys = new ToyList();
   // m_pColors = new ColorList();

#ifdef __HIDAPI__
   hid_init();

   Pinscape::Initialize();
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

#ifdef __HIDAPI__
   IAutoConfigOutputController* items[] = { new PinscapeAutoConfigurator() };

   for (auto& item : items)
   {
      item->AutoConfig(this);
   }
#endif

   Log::Write("Cabinet auto configuration finished");
}

std::string Cabinet::GetConfigXml()
{
   Log::Write("Cabinet::GetConfigXml() Not implemented");

   return "";
}

void Cabinet::SaveConfigXml(const std::string& filename) { Log::Write("Cabinet::SaveConfigXml Not implemented"); }

Cabinet* Cabinet::GetCabinetFromConfigXmlFile(FileInfo* cabinetConfigFile) { return cabinetConfigFile ? GetCabinetFromConfigXmlFile(cabinetConfigFile->FullName()) : nullptr; }

Cabinet* Cabinet::GetCabinetFromConfigXmlFile(const std::string& filename)
{
   std::string xml;

   try
   {
      xml = FileReader::ReadFileToString(filename);

      // For debugging only: copy the contents of the cabinet.xml file to the log, to help diagnose file sourcing issues
      // Log::Write("Read cabinet definition from \"%s\"; file contents follow:\n====\n%s\n====\n\n", filename.c_str(),
      // xml.c_str());
   }
   catch (...)
   {
      Log::Exception("Could not load cabinet config from %s.", filename.c_str());
   }

   return GetCabinetFromConfigXml(xml);
}

Cabinet* Cabinet::GetCabinetFromConfigXml(const std::string& configXml)
{
   /*byte[] xmlBytes = Encoding.Default.GetBytes(ConfigXml);
  using(MemoryStream ms = new MemoryStream(xmlBytes))
  {
    try
    {
      return (Cabinet) new XmlSerializer(typeof(Cabinet)).Deserialize(ms);
    }
    catch (Exception E)
    {
      Exception Ex = new Exception("Could not deserialize the cabinet config from XML data.", E);
      Ex.Data.Add("XML Data", ConfigXml);
      Log.Exception("Could not load cabinet config from XML data.", Ex);
      throw Ex;
    }
  }*/

   return nullptr;
}

bool Cabinet::TestCabinetConfigXmlFile(const std::string& filename)
{
   Log::Write("Not implemented");

   return false;
}

void Cabinet::Init(ICabinetOwner* pCabinetOwner)
{
   Log::Write("Initializing cabinet");
   m_pOwner = pCabinetOwner;
   m_pOutputControllers->Init(this);
   m_pToys->Init(this);

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

} // namespace DOF
