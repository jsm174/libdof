#pragma once

#include "DOF/DOF.h"
#include "../../general/generic/IXmlSerializable.h"

namespace DOF
{

class Cabinet;
class OutputList;

class IOutputController : public virtual IXmlSerializable
{
public:
   IOutputController() { }
   virtual ~IOutputController() { }

   virtual void Init(Cabinet* cabinet) = 0;
   virtual void Finish() = 0;
   virtual void Update() = 0;

   const std::string& GetName() const { return m_name; }
   void SetName(const std::string& name) { m_name = name; }

   virtual OutputList* GetOutputs() = 0;
   virtual const OutputList* GetOutputs() const = 0;

private:
   std::string m_name;
};

}
