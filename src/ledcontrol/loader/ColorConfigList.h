#pragma once

#include <vector>
#include <string>

#include "DOF/DOF.h"
#include "ColorConfig.h"
#include "../../general/color/ColorList.h"

namespace DOF
{

class ColorConfigList
{
public:
   ColorConfigList();
   ~ColorConfigList();

   void Add(ColorConfig* config);
   size_t Size() const { return m_configs.size(); }
   ColorConfig* operator[](size_t index) { return m_configs[index]; }
   const ColorConfig* operator[](size_t index) const { return m_configs[index]; }
   ColorConfig* FindByName(const std::string& name);
   bool Contains(const std::string& name) const;
   void ParseLedControlData(const std::vector<std::string>& ledControlData, bool throwExceptions = false);
   void ParseLedControlData(const std::string& ledControlData, bool throwExceptions = false);
   ColorList GetCabinetColorList() const;

   std::vector<ColorConfig*>::iterator begin() { return m_configs.begin(); }
   std::vector<ColorConfig*>::iterator end() { return m_configs.end(); }
   std::vector<ColorConfig*>::const_iterator begin() const { return m_configs.begin(); }
   std::vector<ColorConfig*>::const_iterator end() const { return m_configs.end(); }

private:
   std::vector<ColorConfig*> m_configs;
};

}