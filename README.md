# Alioth Ultra Performance Kernel

**Device:** POCO F3 / Xiaomi Mi 11X / Redmi K40 (alioth)  
**Platform:** Qualcomm SM8250 (Snapdragon 865/870)  
**Target:** Maximum UI Smoothness & Performance  
**Android Support:** 11 - 16 | HyperOS 3 | AOSP

---

## Features

### Performance Optimizations
- **Input Boost:** Ultra-responsive touchscreen with configurable boost frequencies
- **CPU Boost:** Aggressive CPU frequency scaling for UI threads
- **GPU Boost:** Maximum GPU frequency on touch input
- **RAM Boost:** Optimized zRAM with LZ4/LZ4HC/ZSTD compression
- **Frame Boost:** 60fps frame pacing optimization
- **VSync Control:** Enable/disable VSync dynamically
- **FSync Control:** Enable/disable FSync dynamically

### Root Managers Support
- KernelSU (tiann/KernelSU)
- SukiSU Ultra (with KPM support)
- ReSukiSU
- Magisk (via KernelSU compatibility)
- SuSFS integration

### Franco Kernel Manager Compatibility
All tunables exposed via sysfs:
- `/sys/kernel/alioth_perf/input_boost_duration_ms`
- `/sys/kernel/alioth_perf/input_boost_freq_little`
- `/sys/kernel/alioth_perf/input_boost_freq_big`
- `/sys/kernel/alioth_perf/input_boost_freq_prime`
- `/sys/kernel/alioth_perf/cpu_boost_duration_ms`
- `/sys/kernel/alioth_perf/gpu_boost_freq`
- `/sys/kernel/alioth_perf/gpu_boost_duration_ms`
- `/sys/kernel/alioth_perf/ram_boost_swappiness`
- `/sys/kernel/alioth_perf/vsync_enabled`
- `/sys/kernel/alioth_perf/fsync_enabled`
- `/sys/kernel/alioth_perf/touchscreen_boost`
- `/sys/kernel/alioth_perf/frame_boost`

### Additional Features
- BBR/BBR2 TCP congestion control
- MultiPath TCP support
- WireGuard VPN support
- F2FS with compression support
- EROFS support
- exFAT support
- USB serial drivers (CH340/FTDI/CP210X/PL2303)
- CAN bus support
- Hardware crypto acceleration
- DAMON memory optimization
- Backported BPF for Android 16

---

## Building

### Prerequisites
```bash
sudo apt update
sudo apt install -y build-essential git curl wget bison flex zip bc cpio     libssl-dev ccache tar python3 python-is-python3 libncurses5-dev
```

### Quick Build
```bash
# Clone this repository
git clone https://github.com/YOUR_USERNAME/android_kernel_xiaomi_sm8250.git
cd android_kernel_xiaomi_sm8250

# Build with default settings (SukiSU Ultra)
bash build.sh alioth sukisu

# Build with KernelSU
bash build.sh alioth kernelsu

# Build without root manager
bash build.sh alioth none

# Clean build
bash build.sh alioth sukisu clean
```

### Manual Build
```bash
# Download toolchain
mkdir -p ~/zyc-clang
cd ~/zyc-clang
wget https://github.com/ZyCromerZ/Clang/releases/download/16.0.6-20260510-release/Clang-16.0.6-20260510.tar.gz
tar -xzf Clang-16.0.6-20260510.tar.gz --strip-components=1
cd ..

# Setup environment
export PATH="$HOME/zyc-clang/bin:$PATH"
export CC=clang
export CXX=clang++
export AR=llvm-ar
export NM=llvm-nm
export STRIP=llvm-strip
export OBJCOPY=llvm-objcopy
export OBJDUMP=llvm-objdump
export READELF=llvm-readelf
export CROSS_COMPILE=aarch64-linux-gnu-
export CROSS_COMPILE_ARM32=arm-linux-gnueabi-
export ARCH=arm64

# Generate config
make O=out alioth_perf_defconfig

# Build
make O=out -j$(nproc --all) Image.gz dtb.img dtbo.img
```

### GitHub Actions Build
1. Fork this repository
2. Go to **Actions** tab
3. Select **Build Alioth Ultra Kernel**
4. Click **Run workflow**
5. Select your options and run

---

## Installation

### Prerequisites
- Unlocked bootloader
- Custom recovery (TWRP/OrangeFox)
- Stock DTBO (recommended)

### Flashing
1. Download the zip from releases or artifacts
2. Boot into recovery mode
3. Flash the zip file
4. Wipe cache/dalvik (optional but recommended)
5. Reboot

### Important Notes
- **HyperOS/MIUI users:** Use the MIUI variant (includes Xiaomi display drivers)
- **AOSP users:** Use the AOSP variant
- **DTBO:** Keep stock DTBO for best compatibility. The built DTBO may have brightness issues.
- **First boot:** May take 2-3 minutes after flashing

---

## Configuration

### Franco Kernel Manager
All performance tunables are accessible via Franco Kernel Manager or any kernel manager that supports sysfs interfaces.

### Recommended Settings for Maximum Smoothness
```bash
# Input boost (touchscreen responsiveness)
echo 100 > /sys/kernel/alioth_perf/input_boost_duration_ms
echo 1804800 > /sys/kernel/alioth_perf/input_boost_freq_little
echo 2208000 > /sys/kernel/alioth_perf/input_boost_freq_big
echo 2419200 > /sys/kernel/alioth_perf/input_boost_freq_prime

# GPU boost
echo 800000000 > /sys/kernel/alioth_perf/gpu_boost_freq
echo 100 > /sys/kernel/alioth_perf/gpu_boost_duration_ms

# RAM optimization
echo 60 > /sys/kernel/alioth_perf/ram_boost_swappiness

# Enable all boosts
echo 1 > /sys/kernel/alioth_perf/touchscreen_boost
echo 1 > /sys/kernel/alioth_perf/frame_boost

# VSync/FSync (disable for benchmarking, enable for daily use)
echo 1 > /sys/kernel/alioth_perf/vsync_enabled
echo 1 > /sys/kernel/alioth_perf/fsync_enabled
```

---

## Troubleshooting

### Black screen after flashing
- Check if you flashed the correct variant (MIUI vs AOSP)
- Restore stock DTBO
- Flash stock kernel and try again

### Bootloop
- Boot to recovery
- Flash stock kernel
- Check if root manager is compatible with your ROM

### Performance issues
- Ensure you're using the correct defconfig
- Check thermal throttling
- Verify zRAM is enabled

---

## Credits

- **Base:** [AstideLabs/android_kernel_xiaomi_sm8250](https://github.com/AstideLabs/android_kernel_xiaomi_sm8250)
- **KernelSU:** [tiann/KernelSU](https://github.com/tiann/KernelSU)
- **SukiSU Ultra:** [SukiSU-Ultra/SukiSU-Ultra](https://github.com/SukiSU-Ultra/SukiSU-Ultra)
- **ReSukiSU:** [AstideLabs/ReSukiSU](https://github.com/AstideLabs/ReSukiSU)
- **ZyC-Clang:** [ZyCromerZ/Clang](https://github.com/ZyCromerZ/Clang)
- **LineageOS:** LineageOS Team
- **UtsavBalar1231:** For MIUI/HyperOS drivers

---

## License

This kernel is licensed under GPL v2. See LICENSE file for details.

**Disclaimer:** Flash at your own risk. The developers are not responsible for any damage to your device.
