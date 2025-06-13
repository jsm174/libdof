#include "DOF/Config.h"

namespace DOF
{

Config* Config::m_pInstance = nullptr;

Config* Config::GetInstance()
{
   if (!m_pInstance)
      m_pInstance = new Config();

   return m_pInstance;
}

Config::Config()
{
   m_logLevel = DOF_LogLevel_INFO;
   m_logCallback = nullptr;
}

}
