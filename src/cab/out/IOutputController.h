#pragma once

#include "DOF/DOF.h"

namespace DOF
{

class Cabinet;

class IOutputController
{
public:
   IOutputController() { }
   ~IOutputController() { }

   virtual void Init(Cabinet* pCabinet) = 0;
   virtual void Finish() = 0;
   virtual void Update() = 0;

   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name) { m_name = name; }

private:
   std::string m_name;
};

} // namespace DOF
