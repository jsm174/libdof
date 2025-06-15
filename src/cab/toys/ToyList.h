#pragma once

#include "DOF/DOF.h"
#include "IToy.h"
#include "IToyUpdatable.h"
#include <vector>
#include <string>

namespace DOF
{

class Cabinet;

class ToyList : public std::vector<IToy*>
{
public:
   ToyList();
   ~ToyList();

   void Init(Cabinet* cabinet);
   void Reset();
   void Finish();
   void UpdateToys();
   void UpdateOutputs();
   IToy* FindByName(const std::string& name) const;
   void AddToy(IToy* toy);
   bool RemoveToy(IToy* toy, bool deleteToy = true);
   bool RemoveToy(const std::string& name, bool deleteToy = true);
   bool Contains(const std::string& name) const;

   void WriteXml();
   void ReadXml();
};

}
