#pragma once

#include <string>

namespace DOF
{

class DirectOutputHandler
{
public:
   static std::string GetInstallFolder();

private:
   DirectOutputHandler();
   ~DirectOutputHandler();
};

}