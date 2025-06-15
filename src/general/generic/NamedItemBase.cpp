#include "NamedItemBase.h"

namespace DOF
{

NamedItemBase::NamedItemBase() { }

void NamedItemBase::SetName(const std::string& name)
{
   if (m_name != name)
   {
      std::string oldName = m_name;
      OnBeforeNameChanged(oldName, name);
      BeforeNameChange(oldName, name);
      m_name = name;
      AfterNameChange(oldName, name);
      OnAfterNameChanged(oldName, name);
   }
}

void NamedItemBase::AfterNameChange(const std::string& oldName, const std::string& newName) { }

void NamedItemBase::BeforeNameChange(const std::string& oldName, const std::string& newName) { }

void NamedItemBase::OnAfterNameChanged(const std::string& oldName, const std::string& newName)
{
   if (m_AfterNameChanged.HasHandlers())
   {
      NameChangeEventArgs args(oldName, newName);
      m_AfterNameChanged.Invoke(this, args);
   }
}

void NamedItemBase::OnBeforeNameChanged(const std::string& oldName, const std::string& newName)
{
   if (m_BeforeNameChanged.HasHandlers())
   {
      NameChangeEventArgs args(oldName, newName);
      m_BeforeNameChanged.Invoke(this, args);
   }
}

}