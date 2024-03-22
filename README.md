# libdof

A cross platform library for performing Direct Output Framework tasks.

This library is currently used by [Visual Pinball Standalone](https://github.com/vpinball/vpinball/tree/standalone) to support Direct Output Framework.

## Usage:

```cpp
#include "DOF/DOF.h"
.
.
void setup()
{
   DOF::Config* pConfig = DOF::Config::GetInstance();
   pConfig->;
}

void test()
{
}
```

## dofserver

`dofserver` provides a server process on top of `libdof`.
Per default it listens on port 6789 on localhost and accepts "raw" TCP connections.

`dofserver` accepts these command line options:
* -c --config
    * Config file
    * optional
    * default is no config file
* -a --addr
    * IP address or host name
    * optional
    * default is `localhost`
* -p --port
    * Port
    * optional
    * default is `6789`
* -l --logging
    * Enable logging to stderr
    * optional
    * default is no logging
* -v --verbose
    * Enables verbose logging, includes normal logging
    * optional
    * default is no logging
* -h --help
    * Show help

### Notes

### Examples

### Config File

```ini
[DOFServer]
# The address (interface) to listen for TCP connections.
Addr = localhost
# The port to listen for TCP connections.
Port = 6789
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
