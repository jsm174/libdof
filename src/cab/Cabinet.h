#pragma once

#include "DOF/DOF.h"
#include "ICabinetOwner.h"
#include "../general/generic/IXmlSerializable.h"

namespace DOF
{

class FileInfo;
class CabinetOutputList;
class OutputControllerList;
class ToyList;
class AlarmHandler;
class ScheduledSettings;
class SequentialOutputSettings;

class Cabinet : public IXmlSerializable
{
public:
   Cabinet();
   ~Cabinet();

   void AutoConfig();
   ICabinetOwner* GetOwner() { return m_pOwner; }
   void SetOwner(ICabinetOwner* pOwner) { m_pOwner = pOwner; }
   AlarmHandler* GetAlarms() { return m_pOwner->GetAlarms(); }
   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name) { m_name = name; }
   const std::string& GetCabinetConfigurationFilename() const { return m_cabinetConfigurationFilename; }
   void SetCabinetConfigurationFilename(const std::string& cabinetConfigurationFilename) { m_cabinetConfigurationFilename = cabinetConfigurationFilename; }
   ToyList* GetToys() { return m_pToys; }
   void SetToys(ToyList* pToys) { m_pToys = pToys; }
   bool IsAutoConfigEnabled() const { return m_autoConfigEnabled; }
   void SetAutoConfigEnabled(bool autoConfigEnabled) { m_autoConfigEnabled = autoConfigEnabled; }
   CabinetOutputList* GetOutputs() { return m_pOutputs; }
   OutputControllerList* GetOutputControllers() { return m_pOutputControllers; }
   void SetOutputControllers(OutputControllerList* pOutputControllers) { m_pOutputControllers = pOutputControllers; }
   ScheduledSettings* GetScheduledSettings();
   SequentialOutputSettings* GetSequentialOutputSettings();
   std::string GetConfigXml();
   void SaveConfigXml(const std::string& filename);
   static Cabinet* GetCabinetFromConfigXmlFile(FileInfo* cabinetConfigFile);
   static Cabinet* GetCabinetFromConfigXmlFile(const std::string& filename);
   static Cabinet* GetCabinetFromConfigXml(const std::string& configXml);
   static bool TestCabinetConfigXmlFile(const std::string& filename);
   void Init(ICabinetOwner* pCabinetOwner);
   void Update();
   void Finish();
   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "Cabinet"; }

private:
   ICabinetOwner* m_pOwner;
   std::string m_name;
   std::string m_cabinetConfigurationFilename;
   ToyList* m_pToys;
   bool m_autoConfigEnabled = true;
   CabinetOutputList* m_pOutputs;
   OutputControllerList* m_pOutputControllers;
};

}
