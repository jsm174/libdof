#include "ToyEventArgs.h"
#include "IToy.h"

namespace DOF
{

ToyEventArgs::ToyEventArgs()
   : m_toy(nullptr)
{
}

ToyEventArgs::ToyEventArgs(IToy* toy)
   : m_toy(toy)
{
}

ToyEventArgs::~ToyEventArgs() { }

std::string ToyEventArgs::GetName() const
{
   if (m_toy != nullptr)
   {
      return m_toy->GetName();
   }
   return "";
}

}