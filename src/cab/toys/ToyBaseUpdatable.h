#pragma once

#include "ToyBase.h"
#include "IToyUpdatable.h"

namespace DOF
{

class ToyBaseUpdatable : public ToyBase, public virtual IToyUpdatable
{
public:
   ToyBaseUpdatable() { }
   virtual ~ToyBaseUpdatable() { }

   virtual void UpdateToy() override = 0;
};

}