#pragma once

#include "../../../general/generic/NamedItemList.h"
#include "Shape.h"
#include <string>

namespace DOF
{

class ShapeList : public NamedItemList<Shape>
{
public:
   ShapeList();
   virtual ~ShapeList();

   void WriteXml(std::string& writer);
   void ReadXml(const std::string& reader);
   std::string GetSchema();
};

}