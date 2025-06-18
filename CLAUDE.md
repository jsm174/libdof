# CLAUDE.md

## Project Overview
libdof is a C++ port of the C# DirectOutput Framework achieving 1:1 correspondence. Cross-platform library for Direct Output Framework tasks, used by Visual Pinball Standalone.

**Current Status**: ~85% complete - Core architecture, effects system, device management, addressable LED strips, and toy configuration at 100% 1:1 correspondence

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

# Test specific ROM configuration (sudo required only for PinscapePico)
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
- **Toys System**: All toy types, layers, hardware toys, perfect IRGBOutputToy implementation
- **Configuration**: GlobalConfig, LedControl loader and setup
- **Utilities**: Most general utilities, string extensions, math extensions
- **Output Controllers**: Perfect 1:1 correspondence with C# structure
  - **LedWiz**: Uses LWDEVICE struct pattern matching C# exactly, proper naming conventions
  - **PinscapePico**: ✅ **VERIFIED WORKING** - Uses nested private Device class matching C# pattern, proper toy targeting, fixed matrix effect issues
  - **Pinscape**: Uses nested private Device class matching C# pattern
  - **Auto Configurators**: All follow exact C# patterns with proper variable naming and output naming
- **Logging Infrastructure**: Log::Once() and Log::Instrumentation() methods with perfect C# correspondence
- **Toy Architecture**: ✅ **COMPLETE 1:1 correspondence**:
  - **RGBAToy**: Now implements IRGBOutputToy with SetOutputNameRed/Green/Blue methods matching C# exactly
  - **AnalogAlphaToy**: Perfect layer support with alpha blending matching C# algorithm
  - **Toy Assignment Logic**: Correct case 3 RGB toy creation with existing toy lookup before fallback
  - **Effect Targeting**: Fixed matrix effect targeting - uses AnalogToyValueEffect for single outputs, not matrix effects

#### ⚠️ RECENTLY VERIFIED (High Confidence)
- **PinscapePico Hardware Integration**: ✅ **VERIFIED WORKING** with physical hardware:
  - Proper USB communication with SET OUTPUT PORTS commands
  - Correct fade calculations with alpha blending (L88 fade from 0→3→15→31 values)
  - Working blink+fade effect chain: FullRangeEffect → BlinkEffect → FadeEffect → AnalogToyValueEffect
  - Fixed toy targeting issues - no longer creates incorrect matrix effects for single outputs
- **Effect Chain Creation**: All effect parameters and creation order verified against C#
- **Output Naming**: Fixed critical naming mismatch bugs in PinscapePico (was causing write failures)
- **Addressable LED Strip Controllers**: Complete 1:1 correspondence verification of all 7 files:
  - TeensyStripController: Case-insensitive enum parsing, libserialport integration
  - WemosD1StripController: Perfect RLE compression algorithm, per-strip length features
  - DirectStripController: Threading model with FTDI stubs, proper RAII patterns
  - DirectStripControllerApi: FTDI communication stubs ready for D2XX integration
  - LedStripOutput: Simple wrapper matching C# exactly
  - WS2811StripController: Obsolete wrapper maintaining C# inheritance
  - Custom Parity/StopBits enums with case-insensitive StringExtensions::ToLower parsing
- **LedStrip Toy**: ✅ **COMPLETE 1:1 C# correspondence with successful hardware testing**:
  - TableOverrideSettings and ScheduledSettings integration with proper interface methods
  - GetFadingTableFromPercent() brightness calculation with all curve types (89%, 78%, 67% thresholds)
  - Complex OutputMappingTable structure storing byte offsets (LedNr * 3) matching C# exactly
  - All 6 color order combinations (RGB, RBG, GRB, GBR, BRG, BGR) with exact C# output mapping
  - LedWizEquivalent integration for output strength calculation and overrides
  - Layer blending with AlphaMappingTable using identical alpha compositing algorithm
  - Proper RAII patterns without smart pointers per guidelines
  - **Hardware verified**: TeensyStripController successfully driving physical WS2812 LEDs
  - **Effect verification**: S10→green effects (R0G128B0A255), W88→yellow blink+fade (R31-159G31-159B0)

#### ❌ MISSING CRITICAL COMPONENTS (Major Gap)
**Output Controllers** (50% complete):
- FTDI Controllers (`FTDIChip/`) - CRITICAL
- PAC Controllers (`Pac/`) - PacDrive, PacLed64, PacUIO 
- SSF Controllers (`SSF/`) - 7 variants, feedback systems
- DMX/ArtNet Controllers (`DMX/`) - Professional lighting
- ✅ **Addressable LED Strip Controllers** (`adressableledstrip/`) - **COMPLETE with hardware verification**
- COM Port Controllers (`ComPort/`) - Serial communication
- Philips Hue Controllers (`PhilipsHue/`) - Smart lighting
- PinOne, DudesCab Controllers

**Other Missing**:
- Extensions utilities (`Extensions/` directory - 11 utility classes)
- Missing core files (`Out.cs`, `OutputEventArgs.cs`, namespace files)

### Priority for 1:1 Correspondence
1. **Phase 1**: ✅ **COMPLETE** - Effects system, alarm handling, device management, addressable LED strips, and toy configuration
2. **Phase 2**: Implement FTDI, PAC, SSF, DMX controllers (critical hardware support)
3. **Phase 3**: Complete remaining output controllers  
4. **Phase 4**: Add missing utilities and polish

### Recent Major Achievements (Perfect 1:1 Correspondence)
- **PinscapePico Full Integration**: ✅ **COMPLETE hardware verification**:
  - Fixed toy creation logic in Configurator case 3 - now properly checks for existing IRGBAToy by name before creating new toys
  - Updated RGBAToy to implement IRGBOutputToy interface with SetOutputNameRed/Green/Blue methods
  - Resolved matrix effect targeting issues - single outputs (L88) now use AnalogToyValueEffect instead of AnalogAlphaMatrixValueEffect
  - Verified working USB communication with physical PinscapePico hardware
  - Confirmed proper fade calculations and alpha blending (0→3→15→31 value progression)
  - Perfect effect chain: FullRangeEffect → BlinkEffect → FadeEffect → AnalogToyValueEffect
- **Toy Architecture Completion**: All toy interfaces now match C# exactly:
  - RGBAToy implements IRGBOutputToy with proper output name properties
  - AnalogAlphaToy supports layer blending with correct alpha calculations
  - Toy assignment logic follows C# patterns with existing toy lookup before creation
- **Effect System Verification**: Complete 1:1 correspondence with C# effect creation and targeting
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
- **Device Architecture**: All controllers use exact C# patterns (nested Device classes, struct patterns)
- **Cross-Platform Builds**: Clean compilation on both Windows MSVC and Unix systems
- **Mobile Build Exclusions**: Proper conditional compilation for controllers using external libraries:
  - TeensyStripController and WemosD1StripController excluded on mobile platforms using `#ifdef __LIBSERIALPORT__` in OutputControllerList.cpp
  - HID-based controllers (LedWiz, Pinscape, PinscapePico) excluded using `#ifdef __HIDAPI__`
  - Clean mobile builds (iOS, Android, tvOS) without libserialport/hidapi dependencies
  - Controllers excluded at include level in OutputControllerList.cpp to prevent header dependency issues

### Critical Implementation Notes
- **PinscapePico HID Requirements**: PinscapePico requires sudo access for device enumeration and communication
- **Output Resolution**: Output names must match exactly between auto-configurators and device names
- **Change Detection**: Initialize `m_oldOutputValues` to 0, not 255, to ensure first writes are detected
- **Unit Bias Constants**: Each controller type has specific unit bias (LedWiz: 0, Pinscape: 50, PinscapePico: 119)
- **Device Recognition**: Each controller has specific VID/PID patterns and recognition logic
- **No Comments Rule**: Strictly enforced - no comments in any C++ code
- **Parameter Consistency**: All method parameters use camelCase naming (`cabinet`, `outputValues`, etc.)
- **Static vs Instance**: Use s_ for static members, m_ for instance members
- **XML Namespace**: Always use `tinyxml2::XMLDocument`, `tinyxml2::XMLElement*`, etc.
- **Function Naming**: Avoid Windows API conflicts (use GetDeviceProductName vs GetProductName)
- **Directory Naming**: All new directories should be lowercase (e.g. `adressableledstrip/` not `AdressableLedStrip/`)
- **String Formatting**: StringExtensions::Build() requires all parameters as strings - use `std::to_string()` for integers
- **Serial Communication**: Use libserialport for cross-platform serial port access instead of System.IO.Ports
- **FTDI Integration**: FTDI-based controllers require proprietary FTDI D2XX library/drivers, not libserialport
- **Threading Models**: Use raw pointers with proper RAII patterns instead of shared_ptr/unique_ptr per guidelines
- **Case-Insensitive Enum Parsing**: Use StringExtensions::ToLower() for robust XML enum parsing (Parity, StopBits, etc.)
- **MSVC Array Initialization**: Use double-brace syntax for std::array initialization: `{{0.0f, 0.0f, 0.0f}}` instead of `{0.0f, 0.0f, 0.0f}`
- **Toy Interface Implementation**: All toys must implement correct interfaces (IRGBOutputToy for RGB toys, IAnalogAlphaToy for single output toys)
- **Effect Targeting**: Matrix effects only for matrix toys - use non-matrix effects (AnalogToyValueEffect) for single outputs
- **Mobile Build Pattern**: Exclude controller includes at OutputControllerList.cpp level to prevent transitive header dependencies (e.g., libserialport) on mobile platforms

## References
- **C# source**: `/Users/jmillard/libdof/csharp_code/DirectOutput`
- **Test targets**: `dof_test`, `dof_test_s`, `dofserver`, `dofserver_test`
  - Test INI: `/Users/jmillard/.vpinball/directoutputconfig/directoutputconfig51.ini`
- **Hardware Requirements**: sudo access required for PinscapePico device enumeration

## Memories
- PinscapePico requires sudo access for HID device detection and communication (regular Pinscape works without sudo)
- Always verify effect chain order matches C# exactly: Base → Fade → Blink → Duration → Delay → Invert → FullRange
- RGBAToy now implements IRGBOutputToy interface with proper output name methods
- Matrix effects should only target matrix toys - use AnalogToyValueEffect for single outputs