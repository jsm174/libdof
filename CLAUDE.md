# CLAUDE.md

## Project Overview
libdof is a C++ port of the C# DirectOutput Framework achieving 1:1 correspondence. Cross-platform library for Direct Output Framework tasks, used by Visual Pinball Standalone.

## Core Coding Principles

### 1:1 C# Correspondence Rules
- **Exact Structure**: Match C# namespace, directory structure, file names, method names and order
- **Exact Values**: All enum values, constants, timing values must match C# exactly
- **Value vs Reference Semantics**: C# structs → C++ structs, C# classes → C++ classes
- **Interface Matching**: No extra methods, no missing methods from C# interfaces
- **Algorithm Logic**: Step-based vs time-based approaches must match C# exactly
- **No Invented Code**: Never add validation, guards, or conditional wrapping not present in C#

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
- **OutputList Event Chain**: MUST use `OutputList::Add()` not `push_back()` - Add() automatically subscribes outputs to events for controller notifications

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

### Windows API Conflicts
- Use `SendPipeMessage()` not `SendMessage()`
- Use `GetDeviceProductName()` not `GetProductName()`

### Auto-Configuration Logging Philosophy
- **C# Pattern**: Auto-configurators are mostly silent - only log when devices are actually found/configured
- **No Scan Messages**: C# never logs "device scan found X devices" - remove all such logging
- **Debug vs Production**: Never leave debug logging in production code - maintain 1:1 C# correspondence for all output
