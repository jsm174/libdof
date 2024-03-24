#include "DOF/DOF.h"

#include <algorithm>
#include <chrono>
#include <cstring>

#include "DOF/Config.h"
#include "Logger.h"

#if defined(__APPLE__) && (!((defined(TARGET_OS_IOS) && TARGET_OS_IOS) || (defined(TARGET_OS_TV) && TARGET_OS_TV)))
#include "hidapi.h"
#endif

namespace DOF
{

DOF::DOF()
{
#if defined(__APPLE__) && (!((defined(TARGET_OS_IOS) && TARGET_OS_IOS) || (defined(TARGET_OS_TV) && TARGET_OS_TV)))
  struct hid_device_info *devs, *cur_dev;
  int res = hid_init();

  devs = hid_enumerate(0x0, 0x0);
  cur_dev = devs;

  while (cur_dev)
  {
    std::string productName;
    if (cur_dev->product_string)
    {
      wchar_t* wstr = cur_dev->product_string;
      while (*wstr) productName += static_cast<char>(*wstr++);

      Log("Product: %s", productName.c_str());
    }
    cur_dev = cur_dev->next;
  }
#endif
}

DOF::~DOF() {}

void DOF::DataReceive(char type, int number, int value)
{
  Log("DOF::DataReceive: type=%c, number=%d, value=%d", type, number, value);
}

}  // namespace DOF
