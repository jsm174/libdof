#pragma once

#include "IToy.h"

namespace DOF
{

class ToyBase : public virtual IToy
{
public:
   ToyBase() { }
   virtual ~ToyBase() { }

   virtual void Init(Cabinet* cabinet) override = 0;
   virtual void Reset() override = 0;
   virtual void Finish() override { Reset(); }
};

}