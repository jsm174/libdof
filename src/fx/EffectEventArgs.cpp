#include "EffectEventArgs.h"
#include "IEffect.h"

namespace DOF
{

EffectEventArgs::EffectEventArgs()
   : m_effect(nullptr)
{
}

EffectEventArgs::EffectEventArgs(IEffect* effect)
   : m_effect(effect)
{
}

IEffect* EffectEventArgs::GetEffect() const { return m_effect; }

void EffectEventArgs::SetEffect(IEffect* value) { m_effect = value; }

BeforeEffectNameChangeEventArgs::BeforeEffectNameChangeEventArgs()
   : m_cancelNameChange(false)
{
}

BeforeEffectNameChangeEventArgs::BeforeEffectNameChangeEventArgs(IEffect* effect, const std::string& newName)
   : EffectEventArgs(effect)
   , m_newName(newName)
   , m_cancelNameChange(false)
{
}

const std::string& BeforeEffectNameChangeEventArgs::GetNewName() const { return m_newName; }

void BeforeEffectNameChangeEventArgs::SetNewName(const std::string& value) { m_newName = value; }

bool BeforeEffectNameChangeEventArgs::GetCancelNameChange() const { return m_cancelNameChange; }

void BeforeEffectNameChangeEventArgs::SetCancelNameChange(bool value) { m_cancelNameChange = value; }

}