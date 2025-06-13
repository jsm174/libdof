#include "NameChangeEventArgs.h"

namespace DOF
{

NameChangeEventArgs::NameChangeEventArgs() { }

NameChangeEventArgs::NameChangeEventArgs(const std::string& oldName, const std::string& newName)
   : m_oldName(oldName)
   , m_newName(newName)
{
}

}