#include "DOF/Config.h"

namespace DOF
{

Config* Config::m_pInstance = nullptr;

Config* Config::GetInstance()
{
  if (!m_pInstance) m_pInstance = new Config();

  return m_pInstance;
}

Config::Config()
{
  m_dofServer = false;
  m_dofServerAddr = "localhost";
  m_dofServerPort = 6789;
  m_logCallback = nullptr;
}

}  // namespace DOF
