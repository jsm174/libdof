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

class BeforeEffectNameChangeAventArgs : public EffectEventArgs
{
private:
   std::string m_newName;
   bool m_cancelNameChange;

public:
   BeforeEffectNameChangeAventArgs();
   BeforeEffectNameChangeAventArgs(IEffect* effect, const std::string& newName);
   virtual ~BeforeEffectNameChangeAventArgs() = default;

   const std::string& GetNewName() const;
   void SetNewName(const std::string& value);

   bool GetCancelNameChange() const;
   void SetCancelNameChange(bool value);
};

}