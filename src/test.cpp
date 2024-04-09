#include "test.h"

int main(int argc, const char* argv[])
{
  DOF::Config* pConfig = DOF::Config::GetInstance();
  pConfig->SetLogCallback(LogCallback);

  DOF::DOF* pDOF = new DOF::DOF();

  run(pDOF);

  pDOF->Init("table", "rom");
  pDOF->DataReceive('a', 1, 2);
  pDOF->Finish();

  delete pDOF;

  return 0;
}
