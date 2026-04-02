---
name: review-csharp
description: Review C++ changes against C# DirectOutput source for 1:1 correspondence. Use when reviewing PRs, branches, or specific files.
disable-model-invocation: true
allowed-tools: Read, Grep, Glob, Bash, Agent
argument-hint: "[branch-or-file]"
---

# C# Correspondence Review

Review C++ changes against the C# DirectOutput Framework source at `~/vpx/directoutput` to verify 1:1 correspondence.

## Steps

1. **Get the diff**: If `$ARGUMENTS` is a file path, diff that file. Otherwise, diff the current branch against master:
   ```
   git diff master...HEAD
   ```

2. **For each changed C++ file**, locate the corresponding C# file in `~/vpx/directoutput`. The mapping follows the namespace/directory structure (e.g. `src/fx/matrixfx/` maps to `DirectOutput/FX/MatrixFX/`).

3. **Read both the C++ changes and the full corresponding C# source**, then check for:

### Invented Code
- Validation/clamping logic not present in C#
- Extra conditional checks wrapping code that C# does unconditionally
- Methods, classes, or properties that don't exist in C#
- FadeMode, IsArea guards, or other logic added "for safety" but not in C#
- Extra logging not present in C#

### Missing Code
- Properties set in C# object initializers but missing from C++ (e.g. ActiveValue, InactiveValue)
- Method calls present in C# but absent in C++
- Enum values or constants that C# defines but C++ omits

### Variable Naming
- Methods/functions: PascalCase (e.g. `SetShiftDirection`, `GetAreaSpeed`)
- Parameters and local variables: camelCase (e.g. `activeColor`, `shiftEffect`, `ledWizNr`)
- Instance members: m_ prefix + camelCase (e.g. `m_animationStepSize`)
- Static members: s_ prefix + camelCase
- Variable names must match C# wording converted to camelCase (e.g. C# `TCS` -> C++ `tcs`, C# `ActiveColor` -> C++ `activeColor`, C# `Layer` -> C++ `layer`)

### Value Correspondence
- Validation ranges must match C# exactly (e.g. `.Limit(1, int.MaxValue)` -> `MathExtensions::Limit(value, 1, INT_MAX)`)
- Default values in constructors must match C#
- Timing values must match exactly
- Enum values must match exactly

### Structural Correspondence
- if/else-if chain order must match C#
- Object initializer properties in C# must all be set in C++ (no omissions)
- C# using `XmlSerializer` with reflection is acceptable to implement as manual XML tag checking in C++ (since C++ has no reflection)

## Output Format

For each issue found, report:
- **File and line number** in the C++ code
- **What C# does** (with file and line reference)
- **What C++ does differently**
- **Severity**: `WRONG` (breaks correspondence), `MISSING` (incomplete), `INVENTED` (not in C#), `NAMING` (convention violation)

Summarize with a count of issues by severity at the end.
