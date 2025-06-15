#pragma once

#include <string>
#include "Event.h"
#include "NameChangeEventArgs.h"

namespace DOF
{

class INamedItem
{
public:
   virtual ~INamedItem() = default;

   virtual const std::string& GetName() const = 0;
   virtual void SetName(const std::string& name) = 0;

   Event<NameChangeEventArgs> m_BeforeNameChanged;
   Event<NameChangeEventArgs> m_AfterNameChanged;
};

}