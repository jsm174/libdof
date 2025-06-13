#pragma once

#include "INamedItem.h"
#include <string>

namespace DOF
{

class NamedItemBase : public INamedItem
{
public:
   NamedItemBase();
   virtual ~NamedItemBase() = default;

   const std::string& GetName() const override { return m_name; }
   void SetName(const std::string& name) override;

protected:
   virtual void AfterNameChange(const std::string& oldName, const std::string& newName);
   virtual void BeforeNameChange(const std::string& oldName, const std::string& newName);

private:
   void OnAfterNameChanged(const std::string& oldName, const std::string& newName);
   void OnBeforeNameChanged(const std::string& oldName, const std::string& newName);

   std::string m_name;
};

}