# libdof

A cross platform library for performing Direct Output Framework tasks.

This library is currently used by [Visual Pinball Standalone](https://github.com/vpinball/vpinball/tree/standalone) to support Direct Output Framework.

## About:

**Direct Output Framework (DOF)** is a system originally developed in C# for controlling feedback devices (like LEDs, solenoids, and motors) on virtual pinball machines. It is widely used in the virtual pinball community, especially in combination with Pinscape controllers and B2S-based backglasses.

**libdof** is a cross-platform C++ library aiming to be a faithful 1:1 port of the original [DirectOutput](https://github.com/mjrgh/DirectOutput) C# codebase maintained by [@mjrgh](https://github.com/mjrgh). This work began on March 23, 2024, and builds on previous porting efforts like B2S and FlexDMD, where accuracy to the original code has always been a priority.

> [!WARNING]
> Given the scope of Direct Output Framework - over 250 C# files - this C++ port leveraged a large amount of AI assistance (ChatGPT and Claude). I know for sure it is **NOT** fully accurate and **DOES** contain bugs. Now that most of the files are in place, I will continue refining it until we achieve a true faithful 1:1 port.

As of now, `libdof` can drive LEDs using @mjrgh's excellent [Pinscape](http://mjrnet.org/pinscape) and [Pinscape Pico](https://github.com/mjrgh/PinscapePico) boards, and is in use by [Visual Pinball Standalone](https://github.com/vpinball/vpinball/tree/standalone). 

> [!NOTE]
> On macOS, accessing HID devices with **usage page 0x06** and **usage 0x00** (like Pinscape Pico) requires elevated permissions. You must run as sudo to communicate with the device. See [here](https://gist.github.com/jsm174/d8711b7ea4af4501090f96f1db808202) for more details.

## Usage:

```cpp
#include "DOF/DOF.h"
.
.

void LIBDOFCALLBACK LogCallback(DOF_LogLevel logLevel, const char* format, va_list args)
{
   va_list args_copy;
   va_copy(args_copy, args);
   int size = vsnprintf(nullptr, 0, format, args_copy);
   va_end(args_copy);

   if (size > 0)
   {
      char* const buffer = static_cast<char*>(malloc(size + 1));
      vsnprintf(buffer, size + 1, format, args);
      printf("%s\n", buffer);
      free(buffer);
   }
}

void test()
{
   DOF::Config* pConfig = DOF::Config::GetInstance();
   pConfig->SetLogCallback(LogCallback);
   pConfig->SetLogLevel(DOF_LogLevel_DEBUG);
   pConfig->SetBasePath("/Users/jmillard/.vpinball/");

   DOF::DOF* pDof = new DOF::DOF();
   pDof->Init("", "ij_l7");
   .
   .
   pDof->DataReceive('L', 88, 1);
   .
   .
   pDof->DataReceive('L', 88, 0);
   .
   .
   pDof->Finish();
   delete pDof;
}
```

## Building:

#### Windows (x64)

```shell
platforms/win/x64/external.sh
cmake -G "Visual Studio 17 2022" -DPLATFORM=win -DARCH=x64 -B build
cmake --build build --config Release
```

#### Windows (x86)

```shell
platforms/win/x86/external.sh
cmake -G "Visual Studio 17 2022" -A Win32 -DPLATFORM=win -DARCH=x86 -B build
cmake --build build --config Release
```

#### Linux (x64)
```shell
platforms/linux/x64/external.sh
cmake -DPLATFORM=linux -DARCH=x64 -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
```

#### Linux (aarch64)
```shell
platforms/linux/aarch64/external.sh
cmake -DPLATFORM=linux -DARCH=aarch64 -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
```

#### MacOS (arm64)
```shell
platforms/macos/arm64/external.sh
cmake -DPLATFORM=macos -DARCH=arm64 -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
```

#### MacOS (x64)
```shell
platforms/macos/x64/external.sh
cmake -DPLATFORM=macos -DARCH=x64 -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
```

#### iOS (arm64)
```shell
platforms/ios/arm64/external.sh
cmake -DPLATFORM=ios -DARCH=arm64 -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
```

#### tvOS (arm64)
```shell
platforms/tvos/arm64/external.sh
cmake -DPLATFORM=tvos -DARCH=arm64 -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
```

#### Android (arm64-v8a)
```shell
platforms/android/arm64-v8a/external.sh
cmake -DPLATFORM=android -DARCH=arm64-v8a -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
```
