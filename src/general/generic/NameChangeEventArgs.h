#pragma once

#include <string>

namespace DOF
{

class NameChangeEventArgs
{
public:
   const std::string& GetOldName() const { return m_oldName; }
   void SetOldName(const std::string& oldName) { m_oldName = oldName; }
   const std::string& GetNewName() const { return m_newName; }
   void SetNewName(const std::string& newName) { m_newName = newName; }

   NameChangeEventArgs();
   NameChangeEventArgs(const std::string& oldName, const std::string& newName);

private:
   std::string m_oldName;
   std::string m_newName;
};

}