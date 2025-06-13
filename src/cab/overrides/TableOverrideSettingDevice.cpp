#include "TableOverrideSettingDevice.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include <algorithm>
#include <sstream>

namespace DOF
{

TableOverrideSettingDevice::TableOverrideSettingDevice()
   : m_configPostfixID(0)
   , m_outputPercent(100)
{
}

TableOverrideSettingDevice::TableOverrideSettingDevice(const std::string& name, int configPostfixID, const std::string& outputs, int outputPercent)
   : m_name(name)
   , m_configPostfixID(configPostfixID)
   , m_outputPercent(outputPercent)
{
   Log::Write(StringExtensions::Build("TableOverrideSettingDevice constructor...name={0}", name));
   SetOutputs(outputs);
}

TableOverrideSettingDevice::~TableOverrideSettingDevice() { }

void TableOverrideSettingDevice::SetOutputs(const std::string& outputs)
{
   m_outputs = outputs;
   ParseOutputs();
}

void TableOverrideSettingDevice::ParseOutputs()
{
   m_outputList.clear();

   if (m_outputs.empty())
      return;

   std::stringstream ss(m_outputs);
   std::string item;

   while (std::getline(ss, item, ','))
   {

      item.erase(0, item.find_first_not_of(" \t"));
      item.erase(item.find_last_not_of(" \t") + 1);

      if (!item.empty())
      {
         try
         {
            int output = std::stoi(item);
            m_outputList.push_back(output);
         }
         catch (const std::exception& e)
         {
            Log::Write(StringExtensions::Build("Failed to parse output '{0}' in TableOverrideSettingDevice: {1}", item, std::string(e.what())));
         }
      }
   }
}

}