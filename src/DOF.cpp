#include "DOF/DOF.h"

#include <algorithm>
#include <cctype>
#include <cstring>

#include "Pinball.h"

namespace DOF
{

DOF::DOF() { m_pPinball = new Pinball(); }

DOF::~DOF() { delete m_pPinball; }

void DOF::Init(const char* szTableFilename, const char* szRomName) {}

void DOF::DataReceive(char type, int number, int value) { m_pPinball->ReceiveData(type, number, value); }

void DOF::Finish() {}

}  // namespace DOF
