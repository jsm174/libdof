#include "EffectFactory.h"
#include <algorithm>

namespace DOF
{

EffectFactory& EffectFactory::GetInstance()
{
   static EffectFactory instance;
   return instance;
}

void EffectFactory::RegisterEffect(const std::string& typeName, EffectCreator creator) { m_creators[typeName] = creator; }

IEffect* EffectFactory::CreateEffect(const std::string& typeName) const
{
   auto it = m_creators.find(typeName);
   if (it != m_creators.end())
   {
      return it->second();
   }
   return nullptr;
}

bool EffectFactory::IsEffectRegistered(const std::string& typeName) const { return m_creators.find(typeName) != m_creators.end(); }

std::vector<std::string> EffectFactory::GetRegisteredTypes() const
{
   std::vector<std::string> types;
   types.reserve(m_creators.size());

   for (const auto& pair : m_creators)
   {
      types.push_back(pair.first);
   }

   std::sort(types.begin(), types.end());
   return types;
}

}