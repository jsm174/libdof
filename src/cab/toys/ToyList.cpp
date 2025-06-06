#include "ToyList.h"

#include "../../Log.h"
#include "IToyUpdatable.h"

namespace DOF
{
void ToyList::WriteXml() { Log::Write("Not implemented"); }

void ToyList::ReadXml() { Log::Write("Not implemented"); }

void ToyList::Init(Cabinet* pCabinet)
{
   for (IToy* pToy : *this)
      pToy->Init(pCabinet);
}

void ToyList::Reset()
{
   for (IToy* pToy : *this)
      pToy->Reset();
}

void ToyList::Finish()
{
   for (IToy* pToy : *this)
      pToy->Finish();
}

void ToyList::UpdateOutputs()
{
   for (IToy* pToy : *this)
   {
      IToyUpdatable* pToyUpdatable = dynamic_cast<IToyUpdatable*>(pToy);
      if (pToyUpdatable)
      {
         pToyUpdatable->UpdateOutputs();
      }
   }
}

} // namespace DOF
