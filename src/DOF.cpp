#include "DOF/DOF.h"

#include <algorithm>
#include <cctype>
#include <cstring>

#include "DOF/Config.h"
#include "Logger.h"

#if !(                                                                                                                \
    (defined(__APPLE__) && ((defined(TARGET_OS_IOS) && TARGET_OS_IOS) || (defined(TARGET_OS_TV) && TARGET_OS_TV))) || \
    defined(__ANDROID__) || defined(_WIN32) || defined(_WIN64))
#include "Pinscape.h"
#include "hidapi.h"
#endif

namespace DOF
{

DOF::DOF()
{
#if !(                                                                                                                \
    (defined(__APPLE__) && ((defined(TARGET_OS_IOS) && TARGET_OS_IOS) || (defined(TARGET_OS_TV) && TARGET_OS_TV))) || \
    defined(__ANDROID__) || defined(_WIN32) || defined(_WIN64))
  hid_init();

  Pinscape::Initialize();

  m_pPinscape = new Pinscape(1);
#endif
}

DOF::~DOF()
{
#if !(                                                                                                                \
    (defined(__APPLE__) && ((defined(TARGET_OS_IOS) && TARGET_OS_IOS) || (defined(TARGET_OS_TV) && TARGET_OS_TV))) || \
    defined(__ANDROID__) || defined(_WIN32) || defined(_WIN64))
  delete m_pPinscape;

  hid_exit();
#endif
}

void DOF::DataReceive(char type, int number, int value)
{
  Log("DOF::DataReceive: type=%c, number=%d, value=%d", type, number, value);

  uint8_t outputs[15] = {0};
#if !(                                                                                                                \
    (defined(__APPLE__) && ((defined(TARGET_OS_IOS) && TARGET_OS_IOS) || (defined(TARGET_OS_TV) && TARGET_OS_TV))) || \
    defined(__ANDROID__) || defined(_WIN32) || defined(_WIN64))
  m_pPinscape->UpdateOutputs(outputs);
#endif
}

}  // namespace DOF
