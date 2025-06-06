#pragma once

#include "DOF/DOF.h"

namespace DOF
{

class Cabinet;

class IToy
{
public:
   IToy() { }
   ~IToy() { }

   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name) { m_name = name; }

   virtual void Init(Cabinet* pCabinet) = 0;
   virtual void Reset() = 0;
   virtual void Finish() = 0;

private:
   std::string m_name;
};

} // namespace DOF
