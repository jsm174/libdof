#pragma once

#include "DOF/DOF.h"
#include "IOutputController.h"
#include "../../general/generic/IXmlSerializable.h"

namespace DOF
{

class OutputControllerList : public std::vector<IOutputController*>, public IXmlSerializable
{
public:
   OutputControllerList();
   virtual ~OutputControllerList();

   void Init(Cabinet* pCabinet);
   void Finish();
   void Update();
   bool Contains(const std::string& name) const;
   static IOutputController* CreateController(const std::string& typeName);
   virtual XMLElement* ToXml(XMLDocument& doc) const override;
   virtual bool FromXml(const XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "OutputControllers"; }

private:
   void Clear();
};

}
