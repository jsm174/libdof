# CLAUDE.md

## Project Overview
libdof is a C++ port of the C# DirectOutput Framework achieving 1:1 correspondence. Cross-platform library for Direct Output Framework tasks, used by Visual Pinball Standalone.

**Current Status**: ~99% complete - Core architecture, effects system, device management, all controller types, shape effects with bitmap rendering at 100% 1:1 correspondence.

**Recent Major Implementation**: Matrix toy effects configuration completely fixed to achieve perfect 1:1 C# correspondence - corrected conditional logic for RGBAMatrixColorEffect creation, fixed case-insensitive color lookup, resolved E142/E145 effect creation issues, and cross-platform image loading via stb_image.h for bitmap shapes system.

## Core Coding Principles

### 1:1 C# Correspondence Rules
- **Exact Structure**: Match C# namespace, directory structure, file names, method names and order
- **Exact Values**: All enum values, constants, timing values must match C# exactly
- **Value vs Reference Semantics**: C# structs → C++ structs, C# classes → C++ classes
- **Interface Matching**: No extra methods, no missing methods from C# interfaces
- **Algorithm Logic**: Step-based vs time-based approaches must match C# exactly

### C++ Code Style Requirements
- **Naming**: PascalCase methods, camelCase parameters/variables, m_ instance members, s_ static members
- **Public Fields**: C# public fields → private m_ members + public Get/Set methods
- **No Comments**: Never add comments unless present in C#
- **Variable Names**: Match C# wording but convert to camelCase (`cabinet`, `lw`, `curDev`, `okBecause`, `p` for C# `P`)
- **Method Order**: Match C# order (except constructor/destructor at top for C++)

### Memory Safety Rules
- **TableElementData**: MUST be struct (value type) - never class/pointers
- **Effect Triggering**: AssignedEffectList passes TableElementData by value, not pointer
- **Effect Storage**: All timed effects store TableElementData copies, not pointers
- **Lambda Capture**: Effects lambdas capture by copy `[this]()` - never by reference
- **RAII Patterns**: Raw pointers with proper destructors, no shared_ptr/unique_ptr
- **Value Type Headers**: Files with TableElementData members need full includes

### Critical Implementation Rules
- **Effect Chain Order**: Base → Fade → Blink → Duration → Delay → Invert → FullRange
- **MinDurationEffect**: Must remain active until E149=0 received, not until timer expires
- **AlarmHandler**: Use owner-based registration for interval alarms, not function comparison
- **Device Management**: Nested Device classes for controllers, no separate device files
- **Matrix Targeting**: Matrix effects only for matrix toys, AnalogToyValueEffect for single outputs
- **Change Detection**: Initialize `m_oldOutputValues` to 255 to match C# exactly
- **Timing Values**: Must match C# (e.g. FadeEffect 30ms, MatrixFlicker 30ms interval)

### Cross-Platform Requirements
- **Manual Dependencies**: Build libusb, libftdi, libserialport, hidapi from source
- **Image Loading**: stb_image.h for PNG/GIF/BMP support without external dependencies
- **Conditional Compilation**: `#ifdef __HIDAPI__`, `#ifdef __LIBSERIALPORT__`, `#ifdef __LIBFTDI__`
- **Mobile Build**: Exclude controller includes at OutputControllerList.cpp level
- **Windows Compatibility**: WIN32_LEAN_AND_MEAN, avoid macro conflicts

### Code Formatting Standards
- **Braces**: Omit on single-line if/for (unless logging)
- **Logging**: StringExtensions::Build() with all parameters as strings
- **String Format**: `{0:00}` patterns for zero-padded numbers matching C#
- **XML**: Always `tinyxml2::` prefix, no `using namespace`
- **Arrays**: MSVC requires `{{0.0f, 0.0f}}` for std::array initialization`

### Test ROM Configurations
- **ij_l7**: Blink + Fade effects 
- **gw**: Blink + Fade effects 
- **tna**: Matrix effects
- **bourne**: Bitmap effects
- **goldcue**: Shape effects

## Implementation Status

### ✅ COMPLETE (100% C# Correspondence)
- **Core Framework**: Cabinet, Table, GlobalConfig, Effects system, AlarmHandler
- **Memory Safety**: All heap corruption issues resolved via proper value semantics
- **Effects System**: All effect types with correct chain ordering and nested blink support
- **Toys System**: All toy types with correct interface implementations
- **Output Controllers**: LedWiz, Pinscape, PinscapePico, FTDI, ComPort, DudesCab, DMX, PinOne, LED Strips
- **Matrix Effects**: Flicker, Plasma, and Shift effects with exact timing correspondence
- **Bitmap Effects System**: Complete bitmap loading, FastImage.Frames, DirectOutputShapes.png support
- **Shape Effects System**: SHP code parsing, shape resolution, RGBAMatrixShapeEffect implementation
- **Image Loading System**: Cross-platform image loading via stb_image.h with PNG/GIF/BMP support
- **Configuration**: LedControl loader, GlobalConfig, ScheduledSettings system with corrected matrix effect logic
- **Cross-Platform**: Windows/Linux/macOS builds with manual dependency compilation

### ❌ MISSING COMPONENTS (1% remaining)
- **PAC Controllers**: PacDrive, PacLed64, PacUIO
- **SSF Controllers**: 7 variants with feedback systems  
- **Philips Hue Controllers**: Smart lighting integration
- **Extensions Utilities**: 11 utility classes

## Critical Notes & Memories

### Hardware Requirements
- **PinscapePico**: Requires sudo access for HID enumeration
- **Unit Bias**: LedWiz(0), Pinscape(50), PinscapePico(119), DudesCab(90-94), PinOne(1-16)

### Windows API Conflicts
- Use `SendPipeMessage()` not `SendMessage()` 
- Use `GetDeviceProductName()` not `GetProductName()`

### Memory Corruption Prevention (Critical)
- **Root Cause**: C# structs are value types, C++ classes are reference types
- **TableElementData**: Converted from class to struct to match C# value semantics
- **AssignedEffectList Fix**: Pass TableElementData by value to prevent shared object modification
- **Effects Fix**: All timed effects store copies, preventing heap-use-after-free
- **Thread Safety**: AlarmHandler uses safe callback execution pattern with owner-based registration

### Communication Protocols
- **FTDI**: libftdi1 bitbang at USB level
- **ComPort**: libserialport ASCII `{output},{value}#` format
- **DudesCab**: hidapi HID multi-part messages
- **DMX**: UDP ArtNet broadcast port 6454
- **PinOne**: Named pipes with Base64 encoding over text

### Bitmap Shapes Architecture
- **Image Loading**: Cross-platform loading via stb_image.h supporting PNG, GIF, BMP formats
- **Shape Resolution**: SHP codes resolve to named shapes in DirectOutputShapes.xml
- **Delegation Pattern**: Shape effects delegate to internal bitmap effects (1:1 C# correspondence)
- **Effect Types**: RGBAMatrixShapeEffect, RGBAMatrixColorScaleShapeEffect with animation variants
- **Multi-frame Support**: Image class handles animated GIFs and frame sequences
- **Interface System**: Full IMatrixBitmapEffect hierarchy matching C# exactly

## References
- **C# Source**: `/Users/jmillard/DirectOutput`
- **Documentation**: See NOTES.md for DirectOutput configuration parsing details
