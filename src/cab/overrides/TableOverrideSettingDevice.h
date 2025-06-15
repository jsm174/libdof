#pragma once

#include "DOF/DOF.h"
#include <string>
#include <vector>

namespace DOF
{

class TableOverrideSettingDevice
{
public:
   TableOverrideSettingDevice();
   TableOverrideSettingDevice(const std::string& name, int configPostfixID, const std::string& outputs, int outputPercent);
   ~TableOverrideSettingDevice();

   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name) { m_name = name; }

   int GetConfigPostfixID() const { return m_configPostfixID; }
   void SetConfigPostfixID(int configPostfixID) { m_configPostfixID = configPostfixID; }

   const std::string& GetOutputs() const { return m_outputs; }
   void SetOutputs(const std::string& outputs);

   const std::vector<int>& GetOutputList() const { return m_outputList; }

   int GetOutputPercent() const { return m_outputPercent; }
   void SetOutputPercent(int outputPercent) { m_outputPercent = outputPercent; }

private:
   std::string m_name;
   int m_configPostfixID;
   std::string m_outputs;
   std::vector<int> m_outputList;
   int m_outputPercent;

   void ParseOutputs();
};

}