#pragma once

#include "../general/generic/INamedItem.h"
#include "../general/generic/IXmlSerializable.h"

namespace DOF
{

class Table;
class TableElementData;

class IEffect : public virtual INamedItem, public virtual IXmlSerializable
{
public:
   virtual ~IEffect() = default;

   virtual void Trigger(TableElementData* tableElementData) = 0;
   virtual void Init(Table* table) = 0;
   virtual void Finish() = 0;
};

}
