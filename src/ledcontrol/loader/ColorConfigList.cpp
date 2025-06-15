#include "ColorConfigList.h"
#include "../../Log.h"
#include "../../general/StringExtensions.h"
#include <algorithm>

namespace DOF
{

ColorConfigList::ColorConfigList() { }

ColorConfigList::~ColorConfigList()
{
   for (ColorConfig* config : m_configs)
      delete config;

   m_configs.clear();
}

void ColorConfigList::Add(ColorConfig* config)
{
   if (config != nullptr)
      m_configs.push_back(config);
}

ColorConfig* ColorConfigList::FindByName(const std::string& name)
{
   for (ColorConfig* cc : m_configs)
   {
      if (cc->GetName() == name)
         return cc;
   }
   return nullptr;
}

ColorList ColorConfigList::GetCabinetColorList() const
{
   ColorList cl;
   for (const ColorConfig* cc : m_configs)
      cl.Add(cc->GetCabinetColor());
   return cl;
}

void ColorConfigList::ParseLedControlData(const std::vector<std::string>& ledControlData, bool throwExceptions)
{
   for (const std::string& data : ledControlData)
   {
      if (!StringExtensions::IsNullOrWhiteSpace(data))
         ParseLedControlData(data, throwExceptions);
   }
}

void ColorConfigList::ParseLedControlData(const std::string& ledControlData, bool throwExceptions)
{
   ColorConfig* cc = nullptr;

   try
   {
      cc = new ColorConfig(ledControlData, throwExceptions);
   }
   catch (const std::exception& e)
   {
      Log::Exception(StringExtensions::Build("Could not parse color config data {0}.", ledControlData));
      if (throwExceptions)
         throw std::runtime_error(StringExtensions::Build("Could not parse color config data {0}.", ledControlData));
   }
   if (cc != nullptr)
   {
      if (Contains(cc->GetName()))
      {
         Log::Exception(StringExtensions::Build("Color {0} has already been defined.", cc->GetName()));
         if (throwExceptions)
            throw std::runtime_error(StringExtensions::Build("Color {0} has already been defined.", cc->GetName()));
      }
      else
         Add(cc);
   }
}

bool ColorConfigList::Contains(const std::string& colorName) const
{
   for (const ColorConfig* cc : m_configs)
   {
      if (StringExtensions::ToLower(cc->GetName()) == StringExtensions::ToLower(colorName))
         return true;
   }
   return false;
}

}