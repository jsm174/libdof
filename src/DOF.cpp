#include "DOF/DOF.h"

#include <algorithm>
#include <cctype>
#include <cstring>

#include "Pinball.h"

namespace DOF
{

DOF::DOF() {}

DOF::~DOF() {}

void DOF::Init(const char* szTableFilename, const char* szRomName)
{
  Pinball* pPinball = Pinball::GetInstance();
  pPinball->Setup(/*F.FullName,*/ szTableFilename, szRomName);
  pPinball->Init();
}

void DOF::DataReceive(char type, int number, int value) { Pinball::GetInstance()->ReceiveData(type, number, value); }

void DOF::Finish() { Pinball::GetInstance()->Finish(); }

}  // namespace DOF