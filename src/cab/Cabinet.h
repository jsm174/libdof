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
class CurveList;
class ColorList;
class TableOverrideSettings;

class Cabinet : public IXmlSerializable
{
public:
   Cabinet();
   ~Cabinet();

   void AutoConfig();

   ICabinetOwner* GetOwner() { return m_owner; }
   void SetOwner(ICabinetOwner* owner) { m_owner = owner; }
   AlarmHandler* GetAlarms() { return m_owner->GetAlarms(); }
   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name) { m_name = name; }
   const std::string& GetCabinetConfigurationFilename() const { return m_cabinetConfigurationFilename; }
   void SetCabinetConfigurationFilename(const std::string& cabinetConfigurationFilename) { m_cabinetConfigurationFilename = cabinetConfigurationFilename; }
   ToyList* GetToys() { return m_toys; }
   void SetToys(ToyList* toys) { m_toys = toys; }
   CurveList* GetCurves() { return m_curves; }
   void SetCurves(CurveList* curves) { m_curves = curves; }
   ColorList* GetColors() { return m_colors; }
   void SetColors(ColorList* colors) { m_colors = colors; }
   bool IsAutoConfigEnabled() const { return m_autoConfigEnabled; }
   void SetAutoConfigEnabled(bool autoConfigEnabled) { m_autoConfigEnabled = autoConfigEnabled; }
   CabinetOutputList* GetOutputs() { return m_outputs; }
   OutputControllerList* GetOutputControllers() { return m_outputControllers; }
   void SetOutputControllers(OutputControllerList* outputControllers) { m_outputControllers = outputControllers; }
   ScheduledSettings* GetScheduledSettings();
   SequentialOutputSettings* GetSequentialOutputSettings();
   TableOverrideSettings* GetTableOverrideSettings();
   void SetTableOverrideSettings(TableOverrideSettings* tableOverrideSettings);

   std::string GetConfigXml();
   void SaveConfigXmlFile(const std::string& filename);
   static Cabinet* GetCabinetFromConfigXmlFile(FileInfo* cabinetConfigFile);
   static Cabinet* GetCabinetFromConfigXmlFile(const std::string& filename);
   static Cabinet* GetCabinetFromConfigXml(const std::string& configXml);
   static bool TestCabinetConfigXmlFile(const std::string& filename);

   void Init(ICabinetOwner* cabinetOwner);
   void Update();
   void Finish();

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "Cabinet"; }

private:
   ICabinetOwner* m_owner;
   std::string m_name;
   std::string m_cabinetConfigurationFilename;
   ToyList* m_toys;
   CurveList* m_curves;
   ColorList* m_colors;
   bool m_autoConfigEnabled;
   CabinetOutputList* m_outputs;
   OutputControllerList* m_outputControllers;
};

}