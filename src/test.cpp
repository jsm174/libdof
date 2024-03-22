#include "test.h"

int main(int argc, const char* argv[])
{
  DOF::Config* pConfig = DOF::Config::GetInstance();
  pConfig->SetLogCallback(LogCallback);

  DOF::DOF* pDmd = new DOF::DOF();

  run(pDmd);

  delete pDmd;

  return 0;
}
