#include "test.h"

int main(int argc, const char* argv[])
{
  DOF::Config* pConfig = DOF::Config::GetInstance();
  pConfig->SetLogCallback(LogCallback);
  pConfig->SetDOFServer(true);

  DOF::DOF* pDof = new DOF::DOF();

  run(pDof);

  delete pDof;

  return 0;
}
