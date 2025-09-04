# NOTES.md

## USB Device Access Setup

To use libusb devices (PAC Controllers) without sudo privileges:

### Add user to plugdev group:
```bash
sudo usermod -a -G plugdev $USER
```

### Create udev rules for PAC devices:
```bash
sudo nano /etc/udev/rules.d/99-pacled64.rules
```

Add the following content:
```
# PacLed64 devices (VID:0xD209, PID:0x1401-0x1404)
SUBSYSTEM=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="1401", MODE="0664", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="1402", MODE="0664", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="1403", MODE="0664", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="1404", MODE="0664", GROUP="plugdev"

# PacDrive devices (VID:0xD209, PID:0x1500)
SUBSYSTEM=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="1500", MODE="0664", GROUP="plugdev"
```

### Reload udev rules and logout/login:
```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
# Then logout and login to apply group membership
```

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

## Debug Iteration on Batocera

For testing libdof on Batocera (ARM-based retro gaming distribution), use this cross-compilation and deployment workflow:

### 1. Build on Ubuntu Host
```bash
# Cross-compile for Linux x64 (Batocera target architecture)
cmake -DPLATFORM=linux -DARCH=x64 -B build
cmake --build build -- -j10
```

### 2. Deploy to Batocera Box
```bash
# Sync built libraries and test executable to Batocera
rsync -avz --delete \
  --exclude='libdof.a' \
  --include='lib*' \
  --include='dof_test' \
  --exclude='*' \
  build/ root@192.168.1.160:/userdata/dof
```

### 3. Test on Batocera Target
```bash
# SSH into Batocera box and run tests
ssh root@192.168.1.160
cd /userdata/dof

# Test with twenty4 ROM configuration
./dof_test twenty4 --base-path /userdata/system/configs/vpinball

# Deploy library to VPinballX installation
cp libdof.so.0.3.0 ~/configs/vpinball/VPinballX_GL-10.8.0-2070-c87ffe5-Release-linux-x64/
```

### Notes:
- **Target IP**: `192.168.1.160` - Update to match your Batocera box IP
- **VPinball Path**: Adjust version string (`10.8.0-2070-c87ffe5`) to match your VPinballX build
- **Config Path**: `/userdata/system/configs/vpinball` contains DOF configuration files
- **Architecture**: Batocera typically runs x64 architecture on PC hardware