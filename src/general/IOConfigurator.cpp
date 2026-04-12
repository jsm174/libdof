#include "IOConfigurator.h"

#include "../Log.h"
#include "StringExtensions.h"

#ifdef __HIDAPI__
#include <hidapi/hidapi.h>
#endif

#include <string>

namespace DOF
{

#ifdef __LIBUSB__
libusb_context* IOConfigurator::s_libusbContext = nullptr;
#endif

void IOConfigurator::Initialize()
{
#ifdef __HIDAPI__
   hid_init();
#endif
#ifdef __LIBUSB__
   if (s_libusbContext == nullptr)
   {
      int result = libusb_init(&s_libusbContext);
      if (result < 0)
      {
         Log::Exception(StringExtensions::Build("Failed to initialize libusb: {0}", std::to_string(result)));
         s_libusbContext = nullptr;
      }
   }
#endif
}

void IOConfigurator::Shutdown()
{
#ifdef __LIBUSB__
   if (s_libusbContext != nullptr)
   {
      libusb_exit(s_libusbContext);
      s_libusbContext = nullptr;
   }
#endif
#ifdef __HIDAPI__
   hid_exit();
#endif
}

}
