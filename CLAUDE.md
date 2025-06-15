# CLAUDE.md

## Project Overview
libdof is a C++ port of the C# DirectOutput Framework achieving 1:1 correspondence. Cross-platform library for Direct Output Framework tasks, used by Visual Pinball Standalone.

## Core Principles
- **1:1 Correspondence**: Maintain exact structural correspondence with C# DirectOutput Framework
- **DOF namespace**: All code lives in the DOF namespace  
- **C++ Naming**: Use PascalCase for methods, not camelCase
- **Directory Structure**: Match C# directory structure exactly
- **Method Order**: Keep same order of methods as C# version
- **No Comments**: Do not add comments or implement comments
- **No shared_ptr/make_unique**: Use RAII patterns, proper destructors

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

## References
- **C# source**: `/Users/jmillard/libdof/csharp_code/DirectOutput`
- **Test targets**: `dof_test`, `dof_test_s`, `dofserver`, `dofserver_test`
  - Test INI: `/Users/jmillard/.vpinball/directoutputconfig/directoutputconfig51.ini`