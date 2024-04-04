#include "Pinball.h"

#include <algorithm>
#include <cctype>
#include <cstring>

#include "Logger.h"

#ifdef __HIDAPI__
#include <hidapi/hidapi.h>
#endif

namespace DOF
{

Pinball::Pinball()
{
#ifdef __HIDAPI__
  hid_init();

  // Pinscape::Initialize();

  // m_pPinscape = new Pinscape(1);
#endif
}

Pinball::~Pinball()
{
#ifdef __HIDAPI__
  // delete m_pPinscape;

  hid_exit();
#endif
}

void Pinball::ReceiveData(char type, int number, int value)
{
  Log("Pinball::ReceiveData: type=%c, number=%d, value=%d", type, number, value);

#ifdef __HIDAPI__
  uint8_t outputs[15] = {0};
  // m_pPinscape->UpdateOutputs(outputs);
#endif
}

}  // namespace DOF
