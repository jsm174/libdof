#pragma once

#include <string>

namespace DOF
{

class IEffect;

class EffectEventArgs
{
private:
   IEffect* m_effect;

public:
   EffectEventArgs();
   EffectEventArgs(IEffect* effect);
   virtual ~EffectEventArgs() = default;

   IEffect* GetEffect() const;
   void SetEffect(IEffect* value);
};

class BeforeEffectNameChangeEventArgs : public EffectEventArgs
{
private:
   std::string m_newName;
   bool m_cancelNameChange;

public:
   BeforeEffectNameChangeEventArgs();
   BeforeEffectNameChangeEventArgs(IEffect* effect, const std::string& newName);
   virtual ~BeforeEffectNameChangeEventArgs() = default;

   const std::string& GetNewName() const;
   void SetNewName(const std::string& value);

   bool GetCancelNameChange() const;
   void SetCancelNameChange(bool value);
};

}