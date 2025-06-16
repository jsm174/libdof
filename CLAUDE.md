# CLAUDE.md

## Project Overview
libdof is a C++ port of the C# DirectOutput Framework achieving 1:1 correspondence. Cross-platform library for Direct Output Framework tasks, used by Visual Pinball Standalone.

**Current Status**: ~75% complete - Core architecture, effects system, and device management at 100% 1:1 correspondence, most output controllers missing

## Core Principles
- **1:1 Correspondence**: Maintain exact structural correspondence with C# DirectOutput Framework
- **DOF namespace**: All code lives in the DOF namespace  
- **C++ Naming**: Use PascalCase for methods, not camelCase
- **Parameter Naming**: Use camelCase for method parameters (matching C# converted to C++)
- **Member Variables**: Use m_ prefix for instance members, s_ prefix for static members
- **Directory Structure**: Match C# directory structure exactly
- **Method Order**: Keep same order of methods as C# version
- **No Comments**: Do not add comments or implement comments
- **No shared_ptr/make_unique**: Use RAII patterns, proper destructors
- **Exact Property Names**: C# properties become Get/Set methods with identical names
- **Exact Enum Values**: All enum values must match C# exactly
- **Exact Constants**: All constants (timing, intervals) must match C# values exactly
- **Cross-Platform**: Clean Windows and Unix builds without helper classes or namespace pollution

## Build & Test
```bash
cmake -B build -DPLATFORM=macos -DARCH=arm64
cmake --build build

# Test specific ROM configuration
./build/dof_test ij_l7              # Indiana Jones - Blink + Fade
./build/dof_test test_basic         # Basic L88 - no effects  
./build/dof_test test_fade          # Fade only
./build/dof_test test_blink         # Blink only

# Test all ROM configurations
./build/dof_test
```

## Code Style
- **Braces**: Omit on single-line if/for unless log statement
- **Headers**: Use forward declarations where possible
- **Files**: Headers `.h`, Implementation `.cpp`, match C# names exactly
- **Logging**: Use StringExtensions::Build() for formatted log messages
- **Variable Naming**: Short, descriptive names matching C# patterns (e.g. `cabinet`, `lw`, `curDev`, `okBecause`)
- **String Formatting**: Use `{0:00}` patterns for zero-padded numbers to match C# exactly
- **XML Types**: Always use fully qualified `tinyxml2::` prefixes (no `using namespace`)
- **Windows Compatibility**: Direct API usage with WIN32_LEAN_AND_MEAN, avoid macro conflicts

### Effect Chain Order (Critical)
Effects must be created in this exact order to match C# 1:1:
1. **Base effect** (ValueEffect, AnalogToyValueEffect, etc.)
2. **FadeEffect** (if fade parameters present)
3. **BlinkEffect** (if Blink parameter present)  
4. **DurationEffect** (if duration parameters present)
5. **DelayEffect** (if delay parameters present)
6. **ValueInvertEffect** (if invert parameter present)
7. **ValueMapFullRangeEffect** (always last, handles 0/1→255 conversion)

## Testing Strategy

### Test ROM Configurations
Located in `/Users/jmillard/.vpinball/directoutputconfig/directoutputconfig51.ini`:
- **ij_l7**: `L88 Blink fu500 fd550` - Full effects (original Indiana Jones config)
- **test_basic**: `L88` - No effects
- **test_fade**: `L88 fu500 fd550` - Fade only
- **test_blink**: `L88 Blink` - Blink only  
- **test_both**: `L88 Blink fu500 fd550` - Both effects

### L88 Test Scenarios
Each test ROM runs identical L88 scenarios:
1. **Basic on/off** - 2s fade up + blink, 1.5s fade down
2. **Rapid toggle** - 3x fast on/off cycles (300ms each)
3. **Value comparison** - Test 1→255 conversion vs direct 255
4. **Blink timing** - 3s continuous to verify multiple blinks

## 1:1 Correspondence Guidelines

### Critical Implementation Rules
- **Timing Values**: Must match C# exactly (e.g. FadeEffect uses 30ms, not 20ms)
- **Float vs Int**: Use appropriate types - C# float calculations require C++ float
- **Method Signatures**: Parameters and return types must match C# as closely as possible
- **Enum Correspondence**: Verify all enum values match C# exactly (FadeEffectDurationModeEnum: CurrentToTarget, FullValueRange)
- **Algorithm Logic**: Step-based vs time-based approaches must match C# implementation
- **Interface Matching**: No extra methods beyond C# interface, no missing methods from C# interface
- **Effect Duration Logic**: DurationEffect only for explicit durations (DurationMs > 0) or positive blink counts (Blink > 0), NOT for continuous effects like "Blink" keyword
- **Device Management**: Use C# patterns - nested Device classes for controllers, no separate device files
- **Naming Consistency**: Output names must match between auto-configurators and controllers (e.g. `{0:00}.{1}` format)

### Implementation Status

#### ✅ COMPLETE (100% C# Correspondence)
- **Core Framework**: Cabinet, Table, GlobalConfig, Effects infrastructure
- **Effects System**: All effect types and base classes, correct effect chain creation order
- **AlarmHandler**: Perfect 1:1 interface match - removed extra methods, uses standard RegisterAlarm/UnregisterAlarm
- **FadeEffect**: 30ms intervals, correct duration logic, RegisterIntervalAlarm usage
- **BlinkEffect**: Uses standard alarm methods like C# (RegisterAlarm/UnregisterAlarm)
- **DurationEffect**: Correct trigger logic matching C# exactly, proper retrigger behavior
- **DelayEffect**: Proper alarm cleanup in Finish() method
- **Effect Configuration**: Correct DurationEffect creation logic - only for explicit durations or positive blink counts
- **Toys System**: All toy types, layers, hardware toys
- **Configuration**: GlobalConfig, LedControl loader and setup
- **Utilities**: Most general utilities, string extensions, math extensions
- **Output Controllers**: Perfect 1:1 correspondence with C# structure
  - **LedWiz**: Uses LWDEVICE struct pattern matching C# exactly, proper naming conventions
  - **PinscapePico**: Uses nested private Device class matching C# pattern, fixed output naming bug
  - **Pinscape**: Uses nested private Device class matching C# pattern
  - **Auto Configurators**: All follow exact C# patterns with proper variable naming and output naming
- **Logging Infrastructure**: Log::Once() and Log::Instrumentation() methods with perfect C# correspondence

#### ⚠️ RECENTLY VERIFIED (High Confidence)
- **ValueMapFullRangeEffect**: Cleaned up extra debug logging, pure C# algorithm
- **AnalogToyValueEffect**: Cleaned up extra debug logging, pure C# algorithm  
- **Effect Chain Creation**: All effect parameters and creation order verified against C#
- **Output Naming**: Fixed critical naming mismatch bugs in PinscapePico (was causing write failures)

#### ❌ MISSING CRITICAL COMPONENTS (Major Gap)
**Output Controllers** (25% complete):
- FTDI Controllers (`FTDIChip/`) - CRITICAL
- PAC Controllers (`Pac/`) - PacDrive, PacLed64, PacUIO 
- SSF Controllers (`SSF/`) - 7 variants, feedback systems
- DMX/ArtNet Controllers (`DMX/`) - Professional lighting
- Addressable LED Strip Controllers (`AdressableLedStrip/`) - 6 controller types
- COM Port Controllers (`ComPort/`) - Serial communication
- Philips Hue Controllers (`PhilipsHue/`) - Smart lighting
- PinOne, DudesCab Controllers

**Other Missing**:
- Extensions utilities (`Extensions/` directory - 11 utility classes)
- Missing core files (`Out.cs`, `OutputEventArgs.cs`, namespace files)

### Priority for 1:1 Correspondence
1. **Phase 1**: ✅ **COMPLETE** - Effects system, alarm handling, and device management now have perfect 1:1 correspondence
2. **Phase 2**: Implement FTDI, PAC, SSF, DMX controllers (critical hardware support)
3. **Phase 3**: Complete remaining output controllers  
4. **Phase 4**: Add missing utilities and polish

### Recent Major Achievements (Perfect 1:1 Correspondence)
- **Output Controller 1:1 Correspondence**: All existing controllers (LedWiz, Pinscape, PinscapePico) now have perfect C# correspondence
- **Auto-Configurator Standardization**: All auto-configurators follow exact C# patterns:
  - Parameter naming: `Cabinet` → `cabinet`
  - Variable naming: Short descriptive names (`lw`, `curDev`, `okBecause`, etc.)
  - Output naming: Consistent `{0:00}.{1}` formatting
  - Method structure: Exact C# correspondence
- **Static Member Naming**: Updated to use s_ prefix (s_deviceList, s_devices)
- **Windows Compilation**: Complete fix for MSVC build issues:
  - Removed all `using namespace tinyxml2;` directives
  - Fully qualified all XML types with `tinyxml2::` prefix
  - Fixed macro conflicts by renaming functions (GetDeviceProductName, GetDeviceManufacturerName)
  - Direct Windows API usage without helper classes
- **Critical Bug Fixes**: Fixed PinscapePico output naming mismatch that prevented write commands from working
- **Comment Removal**: All unnecessary comments removed per requirements
- **Device Architecture**: All controllers use exact C# patterns (nested Device classes, struct patterns)
- **Cross-Platform Builds**: Clean compilation on both Windows MSVC and Unix systems

### Critical Implementation Notes
- **Output Resolution**: Output names must match exactly between auto-configurators and device names
- **Change Detection**: Initialize `m_oldOutputValues` to 0, not 255, to ensure first writes are detected
- **Unit Bias Constants**: Each controller type has specific unit bias (LedWiz: 0, Pinscape: 50, PinscapePico: 119)
- **Device Recognition**: Each controller has specific VID/PID patterns and recognition logic
- **No Comments Rule**: Strictly enforced - no comments in any C++ code
- **Parameter Consistency**: All method parameters use camelCase naming (`cabinet`, `outputValues`, etc.)
- **Static vs Instance**: Use s_ for static members, m_ for instance members
- **XML Namespace**: Always use `tinyxml2::XMLDocument`, `tinyxml2::XMLElement*`, etc.
- **Function Naming**: Avoid Windows API conflicts (use GetDeviceProductName vs GetProductName)

## References
- **C# source**: `/Users/jmillard/libdof/csharp_code/DirectOutput`
- **Test targets**: `dof_test`, `dof_test_s`, `dofserver`, `dofserver_test`
  - Test INI: `/Users/jmillard/.vpinball/directoutputconfig/directoutputconfig51.ini`