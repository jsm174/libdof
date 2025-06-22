#include "AssignedEffect.h"
#include "../table/Table.h"
#include "../table/TableElementData.h"
#include "EffectList.h"
#include "../Log.h"
#include "../general/StringExtensions.h"
#include <algorithm>

namespace DOF
{

AssignedEffect::AssignedEffect()
   : m_pEffect(nullptr)
{
   m_effectNameChanged = [this]() { TableElementEffect_EffectNameChanged(); };
}

AssignedEffect::AssignedEffect(const std::string& effectName)
   : m_pEffect(nullptr)
   , m_effectName(effectName)
{
   m_effectNameChanged = [this]() { TableElementEffect_EffectNameChanged(); };
}

void AssignedEffect::SetEffectName(const std::string& effectName)
{
   if (m_effectName != effectName)
   {
      m_effectName = effectName;
      if (m_effectNameChanged)
         m_effectNameChanged();
   }
}

void AssignedEffect::TableElementEffect_EffectNameChanged() { m_pEffect = nullptr; }

void AssignedEffect::ResolveEffectName(Table* table)
{
   m_pEffect = nullptr;

   if (table == nullptr || m_effectName.empty())
      return;

   EffectList* effects = table->GetEffects();
   if (effects == nullptr)
   {
      Log::Write("AssignedEffect: No effect list available");
      return;
   }

   Log::Debug(StringExtensions::Build("AssignedEffect: Looking for effect '{0}' in list of {1} effects", m_effectName, std::to_string(static_cast<int>(effects->size()))));
   if (effects->size() <= 10)
   {
      for (const auto& pair : *effects)
      {
         Log::Debug(StringExtensions::Build("AssignedEffect: Available effect: '{0}'", pair.first));
      }
   }
   auto it = effects->find(m_effectName);
   if (it != effects->end())
   {
      m_pEffect = it->second;
      Log::Debug(StringExtensions::Build("AssignedEffect: Resolved effect '{0}' successfully", m_effectName));
   }
   else
   {
      Log::Write(StringExtensions::Build("AssignedEffect: Failed to resolve effect '{0}'", m_effectName));
   }
}

void AssignedEffect::Trigger(TableElementData* tableElementData)
{
   if (m_pEffect != nullptr && tableElementData != nullptr)
   {
      Log::Debug(StringExtensions::Build("AssignedEffect::Trigger: Triggering effect '{0}' for element {1}{2} with value {3}", m_pEffect->GetName(),
         std::string(1, (char)tableElementData->m_tableElementType), std::to_string(tableElementData->m_number), std::to_string(tableElementData->m_value)));
      try
      {
         m_pEffect->Trigger(tableElementData);
         Log::Debug(StringExtensions::Build("AssignedEffect::Trigger: Effect '{0}' completed", m_pEffect->GetName()));
      }
      catch (const std::exception& ex)
      {
         Log::Exception(StringExtensions::Build("A exception occured when triggering effect {0} for table element {1} {2}. Effect assignement will be deactivated.", m_pEffect->GetName(),
            std::string(1, (char)tableElementData->m_tableElementType), std::to_string(tableElementData->m_number) + " with value " + std::to_string(tableElementData->m_value)));
         m_pEffect = nullptr;
      }
   }
}

void AssignedEffect::Init(Table* table) { ResolveEffectName(table); }

void AssignedEffect::Finish() { m_pEffect = nullptr; }

}