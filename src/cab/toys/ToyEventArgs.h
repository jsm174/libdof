#pragma once

#include <string>

namespace DOF
{

class IToy;

class ToyEventArgs
{
public:
   ToyEventArgs();
   ToyEventArgs(IToy* toy);
   ~ToyEventArgs();

   IToy* GetToy() const { return m_toy; }
   void SetToy(IToy* toy) { m_toy = toy; }
   std::string GetName() const;

private:
   IToy* m_toy;
};

}