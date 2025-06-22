# CLAUDE.md

## Project Overview
libdof is a C++ port of the C# DirectOutput Framework achieving 1:1 correspondence. Cross-platform library for Direct Output Framework tasks, used by Visual Pinball Standalone.

**Current Status**: ~97% complete - Core architecture, effects system, device management, addressable LED strips, FTDI controllers, ComPort controllers, DudesCab controllers, DMX controllers, and toy configuration at 100% 1:1 correspondence. Manual libusb compilation for all platforms eliminates external package dependencies.

## Core Principles

### Structural Correspondence
- **1:1 Correspondence**: Maintain exact structural correspondence with C# DirectOutput Framework
- **DOF namespace**: All code lives in the DOF namespace  
- **Directory Structure**: Match C# directory structure exactly
- **Method Order**: Keep same order of methods as C# version
- **Exact Enum Values**: All enum values must match C# exactly
- **Exact Constants**: All constants (timing, intervals) must match C# values exactly

### C++ Code Style
- **C++ Naming**: Use PascalCase for methods, not camelCase
- **Parameter Naming**: Use camelCase for method parameters (matching C# converted to C++)
- **Member Variables**: Use m_ prefix for instance members, s_ prefix for static members
- **Exact Property Names**: C# properties become Get/Set methods with identical names
- **Variable Naming**: Short, descriptive names matching C# patterns (e.g. `cabinet`, `lw`, `curDev`, `okBecause`)
- **No Comments**: Do not add comments or implement comments

### Critical Implementation Rules
- **Effect Chain Order**: Base → Fade → Blink → Duration → Delay → Invert → FullRange (exact C# order)
- **Timing Values**: Must match C# exactly (e.g. FadeEffect uses 30ms, not 20ms)
- **Algorithm Logic**: Step-based vs time-based approaches must match C# implementation
- **Interface Matching**: No extra methods beyond C# interface, no missing methods from C# interface
- **Device Management**: Use C# patterns - nested Device classes for controllers, no separate device files
- **Effect Targeting**: Matrix effects only for matrix toys - use AnalogToyValueEffect for single outputs
- **Toy Interface Implementation**: All toys must implement correct interfaces (IRGBOutputToy for RGB toys, IAnalogAlphaToy for single output toys)

### Cross-Platform Requirements
- **Cross-Platform**: Clean Windows and Unix builds without helper classes or namespace pollution
- **No shared_ptr/make_unique**: Use RAII patterns, proper destructors
- **Threading Models**: Use raw pointers with proper RAII patterns instead of shared_ptr/unique_ptr
- **Mobile Build Pattern**: Exclude controller includes at OutputControllerList.cpp level to prevent transitive header dependencies
- **Conditional Compilation**: Use `#ifdef __HIDAPI__`, `#ifdef __LIBSERIALPORT__`, `#ifdef __LIBFTDI__` flags properly
- **Manual Dependency Compilation**: All external libraries (libusb, libftdi, libserialport, hidapi) are compiled from source to eliminate package manager dependencies

### Code Formatting
- **Braces**: Omit on single-line if/for unless log statement
- **Headers**: Use forward declarations where possible
- **Files**: Headers `.h`, Implementation `.cpp`, match C# names exactly
- **Logging**: Use StringExtensions::Build() for formatted log messages
- **String Formatting**: Use `{0:00}` patterns for zero-padded numbers to match C# exactly
- **XML Types**: Always use fully qualified `tinyxml2::` prefixes (no `using namespace`)
- **Windows Compatibility**: Direct API usage with WIN32_LEAN_AND_MEAN, avoid macro conflicts
- **Case-Insensitive Enum Parsing**: Use StringExtensions::ToLower() for robust XML enum parsing

## Build & Test
```bash
# Build on any platform
cmake -B build -DPLATFORM=macos -DARCH=arm64
cmake --build build

# Test with default configuration path
# Windows: current directory
# Linux/macOS: ~/.vpinball/
./build/dof_test

# Test specific ROM configuration
./build/dof_test ij_l7              # Indiana Jones - Blink + Fade
./build/dof_test test_basic         # Basic L88 - no effects  
./build/dof_test test_fade          # Fade only
./build/dof_test test_blink         # Blink only

# Test with custom base path
./build/dof_test --base-path /path/to/config/
./build/dof_test --base-path /tmp/test/ ij_l7

# Show help
./build/dof_test --help
```

## Testing Strategy

### Test ROM Configurations
Located in configuration directory (default: `~/.vpinball/directoutputconfig/directoutputconfig51.ini`):
- **ij_l7**: `L88 Blink fu500 fd550` - Full effects (original Indiana Jones config)
- **test_basic**: `L88` - No effects
- **test_fade**: `L88 fu500 fd550` - Fade only
- **test_blink**: `L88 Blink` - Blink only  
- **test_both**: `L88 Blink fu500 fd550` - Both effects

Use `--base-path` to specify custom configuration directory.

### L88 Test Scenarios
Each test ROM runs identical L88 scenarios:
1. **Basic on/off** - 2s fade up + blink, 1.5s fade down
2. **Rapid toggle** - 3x fast on/off cycles (300ms each)
3. **Value comparison** - Test 1→255 conversion vs direct 255
4. **Blink timing** - 3s continuous to verify multiple blinks

## Implementation Status

### ✅ COMPLETE (100% C# Correspondence)
- **Core Framework**: Cabinet, Table, GlobalConfig, Effects infrastructure
- **Effects System**: All effect types and base classes, correct effect chain creation order
- **AlarmHandler**: Perfect 1:1 interface match - removed extra methods, uses standard RegisterAlarm/UnregisterAlarm
- **Toys System**: All toy types, layers, hardware toys, perfect IRGBOutputToy implementation
- **Configuration**: GlobalConfig, LedControl loader and setup
- **Utilities**: Most general utilities, string extensions, math extensions
- **Output Controllers**: Perfect 1:1 correspondence with C# structure
  - **LedWiz**: Uses LWDEVICE struct pattern matching C# exactly
  - **PinscapePico**: ✅ **VERIFIED WORKING** - Uses nested private Device class, proper toy targeting
  - **Pinscape**: Uses nested private Device class matching C# pattern
  - **FTDI Controllers**: ✅ **COMPLETE** - FT245RBitbangController with libftdi integration
  - **ComPort Controllers**: ✅ **COMPLETE** - PinControl with libserialport integration  
  - **DudesCab Controllers**: ✅ **COMPLETE** - RP2040-based controller with hidapi integration
  - **DMX Controllers**: ✅ **COMPLETE** - ArtNet controller with cross-platform UDP networking
  - **Addressable LED Strip Controllers**: ✅ **COMPLETE** - All 7 files with hardware verification
  - **Auto Configurators**: All follow exact C# patterns with proper variable naming and output naming
- **Logging Infrastructure**: Log::Once() and Log::Instrumentation() methods with perfect C# correspondence

### ❌ MISSING COMPONENTS (3% remaining)
**Output Controllers** (90% complete):
- PAC Controllers (`Pac/`) - PacDrive, PacLed64, PacUIO 
- SSF Controllers (`SSF/`) - 7 variants, feedback systems  
- Philips Hue Controllers (`PhilipsHue/`) - Smart lighting
- PinOne Controllers

**Other Missing**:
- Extensions utilities (`Extensions/` directory - 11 utility classes)

### Priority for 1:1 Correspondence
1. **Phase 1**: ✅ **COMPLETE** - Effects system, alarm handling, device management, addressable LED strips, and toy configuration
2. **Phase 2**: ✅ **COMPLETE** - FTDI, ComPort, DudesCab, and DMX controllers with cross-platform implementation
3. **Phase 3**: Complete remaining output controllers (PAC, SSF, PhilipsHue, PinOne)
4. **Phase 4**: Add missing utilities and polish

## Recent Major Achievements

### Build System Modernization: ✅ **COMPLETE**
- **Manual Dependency Compilation**: Eliminated all external package manager dependencies (MSYS2, pacman, apt)
- **Windows**: Uses native Visual Studio toolchain to build libusb, libftdi, libserialport, hidapi from source
- **Linux/macOS**: Uses autotools and CMake to build all dependencies with proper cross-compilation support
- **GitHub CI**: Updated workflows to remove MSYS2 dependencies and use native toolchains for all platforms
- **Consistent Builds**: Pinned commit hashes ensure reproducible builds across all environments
- **Cross-platform libftdi**: Direct libusb integration for FTDI controllers without external package conflicts

### Test Application Enhancement: ✅ **COMPLETE**
- **Command Line Interface**: Added `--base-path` and `--help` arguments with clean `std::cout` output
- **Platform-Specific Defaults**: Windows uses current directory, Linux/macOS use `~/.vpinball/`
- **Flexible Testing**: Support for custom configuration paths and specific ROM testing
- **Clean Output**: Separated CLI messages (cout) from DOF runtime logging for better user experience

### DMX Controllers Implementation: ✅ **COMPLETE**
- **DMX.h/.cpp**: Namespace definition files for DMX controllers
- **DMXOutput.h/.cpp**: DMX output class with channel validation (1-512) and auto-naming
- **ArtNet.h/.cpp**: Main ArtNet controller with exact C# correspondence - 512 DMX channels per universe
- **Engine.h/.cpp**: Cross-platform ArtNet engine using standard UDP sockets (no external dependencies)
- Perfect ArtNet protocol implementation with UDP broadcast to port 6454
- Cross-platform networking: Windows (winsock2) and Unix (sys/socket) with proper error handling
- Singleton pattern for engine instance with thread-safe initialization
- Manual XML configuration only (no auto-configurator needed)
- Complete OutputControllerList integration with factory pattern

### DudesCab Controllers Implementation: ✅ **COMPLETE**
- **DudesCab.h/.cpp**: RP2040-based controller with exact C# correspondence - 128 PWM outputs through extension boards
- Uses hidapi for cross-platform HID communication with proper string handling for Windows/Unix
- Perfect HID protocol implementation with multi-part message support matching C# exactly
- Conditional compilation with `#ifdef __HIDAPI__` flag for mobile platform exclusion
- **DudesCabAutoConfigurator.h/.cpp**: Auto-detection with units 90-94 and LedWizEquivalent creation
- Cross-platform string conversion handling for hidapi wide/UTF-8 string differences
- Complete OutputControllerList integration with factory pattern

### ComPort Controllers Implementation: ✅ **COMPLETE** 
- **PinControl.h/.cpp**: Arduino-based controller with exact C# correspondence - 10 outputs (4 PWM + 6 digital)
- Uses libserialport for cross-platform serial communication (115200 baud, 8N1)
- Perfect ASCII protocol implementation: `{outputNumber},{value}#` format matching C# exactly

### FTDI Controllers Implementation: ✅ **COMPLETE**
- **FTDI.h/.cpp**: C++ wrapper for libftdi functionality replacing C# FTD2XX_NET.cs with exact API correspondence
- **FT245RBitbangController.h/.cpp**: Main FTDI controller with threading model, XML serialization, and bit manipulation logic
- Uses libftdi1 for cross-platform USB communication with exact C# structure and behavior

## Critical Implementation Notes
- **PinscapePico HID Requirements**: PinscapePico requires sudo access for device enumeration and communication
- **DudesCab HID Requirements**: DudesCab requires hidapi for cross-platform HID communication with proper wide string handling
- **Output Resolution**: Output names must match exactly between auto-configurators and device names
- **Change Detection**: Initialize `m_oldOutputValues` to 0, not 255, to ensure first writes are detected
- **Unit Bias Constants**: Each controller type has specific unit bias (LedWiz: 0, Pinscape: 50, PinscapePico: 119, DudesCab: 90-94)
- **Device Recognition**: Each controller has specific VID/PID patterns and recognition logic
- **Parameter Consistency**: All method parameters use camelCase naming (`cabinet`, `outputValues`, etc.)
- **Static vs Instance**: Use s_ for static members, m_ for instance members
- **Function Naming**: Avoid Windows API conflicts (use GetDeviceProductName vs GetProductName)
- **Directory Naming**: All new directories should be lowercase (e.g. `dudescab/` not `DudesCab/`)
- **String Formatting**: StringExtensions::Build() requires all parameters as strings - use `std::to_string()` for integers, supports vector<string> overload for >4 parameters
- **Serial Communication**: Use libserialport for cross-platform serial port access instead of System.IO.Ports
- **FTDI Integration**: FTDI-based controllers use libftdi1 for cross-platform USB communication with exact C# API correspondence
- **MSVC Array Initialization**: Use double-brace syntax for std::array initialization: `{{0.0f, 0.0f, 0.0f}}` instead of `{0.0f, 0.0f, 0.0f}`

## References
- **C# source**: `/Users/jmillard/libdof/csharp_code/DirectOutput`
- **Test targets**: `dof_test`, `dof_test_s`, `dofserver`, `dofserver_test`
  - Test INI: Default `~/.vpinball/directoutputconfig/directoutputconfig51.ini` or custom path via `--base-path`
- **Hardware Requirements**: sudo access required for PinscapePico device enumeration
- **Build Dependencies**: All external libraries built from source - no package manager dependencies required

## Memories
- PinscapePico requires sudo access for HID device detection and communication (regular Pinscape works without sudo)
- Always verify effect chain order matches C# exactly: Base → Fade → Blink → Duration → Delay → Invert → FullRange
- RGBAToy now implements IRGBOutputToy interface with proper output name methods
- Matrix effects should only target matrix toys - use AnalogToyValueEffect for single outputs
- DudesCab controllers use units 90-94 in DOF config, require hidapi for cross-platform HID communication
- StringExtensions::Build() supports vector<string> overload for more than 4 parameters