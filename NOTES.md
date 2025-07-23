# NOTES.md

## DirectOutput Framework Configuration & Debugging

### Official DirectOutput Documentation
- **Main Reference**: https://directoutput.github.io/DirectOutput/inifiles.html#inifiles_triggerpara
- **Parameter Reference**: Complete list of all configuration parameters and their meanings

## ROM Configuration Parsing Flow

The DirectOutput Framework parses ROM configurations through a multi-stage process:

### 1. Configuration File Loading
```
~/.vpinball/directoutputconfig/directoutputconfig51.ini (ROM config file)
    ↓
LedControl::LoadFromFiles() 
    ↓  
TableConfig (per ROM like "tna")
    ↓
TableConfigSetting (per line like "E144 Yellow AL0 AT0 F100...")
    ↓
TableConfigColumn (individual effects parsed out)
    ↓
Individual Effect objects created
```

### 2. Key Parsing Files
- **`src/ledcontrol/loader/LedControl.cpp`** - Main parser that reads ROM configurations
- **`src/ledcontrol/loader/TableConfig.cpp`** - Parses individual table configurations  
- **`src/ledcontrol/loader/TableConfigSetting.cpp`** - Parses individual effect settings
- **`src/ledcontrol/loader/TableConfigColumn.cpp`** - Creates individual Effect objects

## Memory Leak Detection & Build Tools

### Advanced Memory Debugging (LLVM clang)
macOS system clang doesn't support AddressSanitizer leak detection. Use LLVM clang for comprehensive memory leak analysis:

```bash
# Special build with LLVM clang for AddressSanitizer leak detection
cmake -DPLATFORM=macos -DARCH=arm64 \
  -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm/bin/clang \
  -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ \
  -DENABLE_SANITIZERS=ON \
  -DCMAKE_BUILD_TYPE=Debug \
  -B build-clang
cmake --build build-clang -- -j10
```

### Memory Leak Detection Methods
```bash
# Method 1: AddressSanitizer (comprehensive, real-time detection)
ASAN_OPTIONS=detect_leaks=1 ./build-clang/dof_test tna

# Method 2: macOS leaks tool (at exit)
leaks --atExit -- ./build/dof_test tna

# Method 3: macOS leaks tool (live process)
./build/dof_test tna &
PID=$!
sleep 2
leaks $PID
kill $PID
```