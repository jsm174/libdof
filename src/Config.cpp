#include "DOF/Config.h"

namespace DOF
{

Config* Config::m_instance = nullptr;

Config* Config::GetInstance()
{
   if (!m_instance)
      m_instance = new Config();

   return m_instance;
}

Config::Config()
{
   m_logLevel = DOF_LogLevel_INFO;
   m_logCallback = nullptr;
}

}
