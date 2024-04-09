#pragma once

#include "DOF/DOF.h"
#include "ICabinetOwner.h"

namespace DOF
{

class OutputControllerList;
class ToyList;
class AlarmHandler;

class Cabinet
{
 public:
  Cabinet();
  ~Cabinet();

  void AutoConfig();
  ICabinetOwner* GetOwner() { return m_pOwner; }
  void SetOwner(ICabinetOwner* pOwner) { m_pOwner = pOwner; }
  AlarmHandler* GetAlarms() { return m_pOwner->GetAlarms(); }
  const std::string& GetName() const { return m_szName; }
  void SetName(const std::string& szName) { m_szName = szName; }
  const std::string& GetCabinetConfigurationFilename() const { return m_szCabinetConfigurationFilename; }
  void SetCabinetConfigurationFilename(const std::string& szCabinetConfigurationFilename)
  {
    m_szCabinetConfigurationFilename = szCabinetConfigurationFilename;
  }
  ToyList* GetToys() { return m_pToys; }
  void SetToys(ToyList* pToys) { m_pToys = pToys; }
  // GetColorList
  // SetColorList
  // GetCurveList
  // SetCurveList
  bool GetAutoConfiguredEnabled() const { return m_autoConfiguredEnabled; }
  void SetAutoConfiguredEnabled(bool autoConfiguredEnabled) { m_autoConfiguredEnabled = autoConfiguredEnabled; }
  // CabinetOutputList
  OutputControllerList* GetOutputControllers() { return m_pOutputControllers; }
  void SetOutputControllers(OutputControllerList* pOutputControllers) { m_pOutputControllers = pOutputControllers; }
  // GetScheduledSettings
  // SetScheduledSettings
  // GetSequentialOutputSettings
  // SetSequentialOutputSettings
  // GetTableOverrideSettings
  // SetTableOverrideSettings
  std::string GetConfigXml();
  void SaveConfigXml(const std::string& szFileName);
  static Cabinet* GetCabinetFromConfigXmlFile(const std::string& szFileName);
  static bool TestCabinetConfigXmlFile(const std::string& szFileName);
  void Init(ICabinetOwner* pCabinetOwner);
  void Update();
  void Finish();

 private:
  ICabinetOwner* m_pOwner;
  std::string m_szName;
  std::string m_szCabinetConfigurationFilename;
  ToyList* m_pToys;
  bool m_autoConfiguredEnabled = true;
  OutputControllerList* m_pOutputControllers;
};

}  // namespace DOF
