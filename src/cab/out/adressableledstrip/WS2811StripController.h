#pragma once

#include "DOF/DOF.h"
#include "DirectStripController.h"

namespace DOF
{

class WS2811StripController : public DirectStripController
{
public:
   WS2811StripController() = default;
   virtual ~WS2811StripController() = default;

   virtual std::string GetXmlElementName() const override { return "WS2811StripController"; }
};

}