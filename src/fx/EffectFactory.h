#pragma once

#include "IEffect.h"
#include <unordered_map>
#include <string>
#include <functional>
#include <memory>

namespace DOF
{

class EffectFactory
{
public:
   using EffectCreator = std::function<IEffect*()>;

   static EffectFactory& GetInstance();


   void RegisterEffect(const std::string& typeName, EffectCreator creator);


   IEffect* CreateEffect(const std::string& typeName) const;


   bool IsEffectRegistered(const std::string& typeName) const;


   std::vector<std::string> GetRegisteredTypes() const;

private:
   EffectFactory() = default;
   std::unordered_map<std::string, EffectCreator> m_creators;
};


#define REGISTER_EFFECT(EffectClass, XmlElementName)                                                                                                                                         \
   namespace                                                                                                                                                                                 \
   {                                                                                                                                                                                         \
      static bool g_##EffectClass##_registered = []()                                                                                                                                        \
      {                                                                                                                                                                                      \
         EffectFactory::GetInstance().RegisterEffect(XmlElementName, []() -> IEffect* { return static_cast<IEffect*>(new EffectClass()); });                                                 \
         return true;                                                                                                                                                                        \
      }();                                                                                                                                                                                   \
   }


#define REGISTER_MATRIX_EFFECT(EffectClass, XmlElementName)                                                                                                                                  \
   namespace                                                                                                                                                                                 \
   {                                                                                                                                                                                         \
      static bool g_##EffectClass##_registered = []()                                                                                                                                        \
      {                                                                                                                                                                                      \
         EffectFactory::GetInstance().RegisterEffect(XmlElementName,                                                                                                                         \
            []() -> IEffect*                                                                                                                                                                 \
            {                                                                                                                                                                                \
               EffectClass* effect = new EffectClass();                                                                                                                                      \
               return static_cast<EffectBase*>(effect);                                                                                                                                      \
            });                                                                                                                                                                              \
         return true;                                                                                                                                                                        \
      }();                                                                                                                                                                                   \
   }

}