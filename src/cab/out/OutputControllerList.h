#pragma once

#include "DOF/DOF.h"
#include "IOutputController.h"

namespace DOF
{

class OutputControllerList : public std::vector<IOutputController*>
{
public:
   void WriteXml();
   void ReadXml();
   void Init(Cabinet* pCabinet);
   void Finish();
   void Update();
   bool Contains(const std::string& name) const;
};

} // namespace DOF
