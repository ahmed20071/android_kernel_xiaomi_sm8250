# Alioth Ultra Performance Kernel - Complete Documentation

## Table of Contents
1. [Overview](#overview)
2. [Features](#features)
3. [Installation](#installation)
4. [Configuration](#configuration)
5. [Franco Kernel Manager Compatibility](#franco-kernel-manager-compatibility)
6. [Troubleshooting](#troubleshooting)
7. [Building from Source](#building-from-source)
8. [Advanced Tuning](#advanced-tuning)

## Overview

Alioth Ultra Performance Kernel is a custom kernel specifically designed for the POCO F3 / Xiaomi Mi 11X / Redmi K40 (alioth) running on the Qualcomm SM8250 platform. The kernel is optimized for maximum UI smoothness and performance across Android 11-16, HyperOS 3, and AOSP-based custom ROMs.

## Features

### Performance Features
- **Input Boost**: Ultra-responsive touchscreen with configurable boost frequencies
- **CPU Boost**: Aggressive CPU frequency scaling for UI threads
- **GPU Boost**: Maximum GPU frequency on touch input
- **RAM Boost**: Optimized zRAM with multiple compression algorithms
- **Frame Boost**: 60fps frame pacing optimization
- **VSync Control**: Enable/disable VSync dynamically via sysfs
- **FSync Control**: Enable/disable FSync dynamically via sysfs

### Root Manager Support
- KernelSU (tiann/KernelSU)
- SukiSU Ultra (with KPM support)
- ReSukiSU
- Magisk (via KernelSU compatibility)
- SuSFS integration

### Hardware Support
- Adreno 650 GPU with full frequency control
- Kryo 585 CPU with per-cluster governor control
- 120Hz display with optimized timing
- WiFi 6 support
- Hi-Res audio (192kHz/32-bit)
- Fast charging (up to 6A)
- USB 3.0 with fast charge

### File System Support
- F2FS with compression (LZ4/LZ4HC/ZSTD)
- EROFS
- exFAT
- NTFS

## Installation

### Prerequisites
- Unlocked bootloader
- Custom recovery (TWRP/OrangeFox)
- Stock DTBO (recommended)

### Flashing Steps
1. Download the appropriate zip file for your ROM
2. Boot into recovery mode
3. Flash the zip file
4. Wipe cache/dalvik (optional but recommended)
5. Reboot

### Important Notes
- **HyperOS/MIUI users**: Use the MIUI variant
- **AOSP users**: Use the AOSP variant
- **DTBO**: Keep stock DTBO for best compatibility
- **First boot**: May take 2-3 minutes after flashing

## Configuration

### Default Performance Settings
The kernel comes with optimized default settings:
- CPU Governor: Performance
- GPU Governor: Performance
- I/O Scheduler: BFQ
- Swappiness: 60
- Input Boost: Enabled
- Frame Boost: Enabled
- VSync: Enabled
- FSync: Enabled

### Sysfs Interfaces

#### CPU Control (`/sys/kernel/alioth_cpu/`)
- `little_governor` - Governor for little cores
- `big_governor` - Governor for big cores
- `prime_governor` - Governor for prime core
- `little_min_freq` - Minimum frequency for little cores
- `little_max_freq` - Maximum frequency for little cores
- `big_min_freq` - Minimum frequency for big cores
- `big_max_freq` - Maximum frequency for big cores
- `prime_min_freq` - Minimum frequency for prime core
- `prime_max_freq` - Maximum frequency for prime core
- `cpu_boost_freq_little` - Boost frequency for little cores
- `cpu_boost_freq_big` - Boost frequency for big cores
- `cpu_boost_freq_prime` - Boost frequency for prime core
- `cpu_boost_duration_ms` - Duration of CPU boost
- `schedutil_up_rate_limit_us` - Schedutil up rate limit
- `schedutil_down_rate_limit_us` - Schedutil down rate limit
- `schedutil_hispeed_freq` - Schedutil hispeed frequency
- `schedutil_hispeed_load` - Schedutil hispeed load
- `interactive_above_hispeed_delay` - Interactive above hispeed delay
- `interactive_go_hispeed_load` - Interactive go hispeed load
- `interactive_timer_rate` - Interactive timer rate

#### GPU Control (`/sys/kernel/alioth_gpu/`)
- `gpu_boost_freq` - GPU boost frequency
- `gpu_boost_duration_ms` - GPU boost duration
- `gpu_min_freq` - Minimum GPU frequency
- `gpu_max_freq` - Maximum GPU frequency
- `gpu_freq_table` - Available GPU frequencies
- `gpu_default_pwrlevel` - Default power level
- `gpu_throttle_enable` - Enable/disable throttling
- `gpu_bus_split` - Bus split setting
- `gpu_idle_timer` - GPU idle timer
- `gpu_governor` - GPU governor
- `gpu_preemption_preemptlevel` - Preemption level

#### RAM Control (`/sys/kernel/alioth_ram/`)
- `adaptive_swappiness` - Enable adaptive swappiness
- `base_swappiness` - Base swappiness value
- `zram_disksize_pct` - zRAM size as percentage of RAM
- `zram_comp_algorithm` - zRAM compression algorithm
- `min_free_kbytes` - Minimum free memory
- `vfs_cache_pressure` - VFS cache pressure
- `dirty_ratio` - Dirty ratio
- `dirty_background_ratio` - Dirty background ratio

#### Performance Control (`/sys/kernel/alioth_perf/`)
- `input_boost_duration_ms` - Input boost duration
- `input_boost_freq_little` - Input boost frequency (little)
- `input_boost_freq_big` - Input boost frequency (big)
- `input_boost_freq_prime` - Input boost frequency (prime)
- `cpu_boost_duration_ms` - CPU boost duration
- `gpu_boost_freq` - GPU boost frequency
- `gpu_boost_duration_ms` - GPU boost duration
- `ram_boost_swappiness` - RAM boost swappiness
- `vsync_enabled` - Enable/disable VSync
- `fsync_enabled` - Enable/disable FSync
- `touchscreen_boost` - Enable/disable touchscreen boost
- `frame_boost` - Enable/disable frame boost

## Franco Kernel Manager Compatibility

This kernel is fully compatible with Franco Kernel Manager. All tunables are exposed via sysfs and can be controlled through the app.

### Recommended FKM Settings

#### CPU Tab
- Governor: Performance (for maximum smoothness)
- Min/Max frequencies: Set according to your thermal preferences
- Boost frequencies: 1804MHz (little), 2208MHz (big), 2419MHz (prime)

#### GPU Tab
- Governor: Performance
- Min frequency: 220MHz
- Max frequency: 900MHz
- Boost frequency: 900MHz

#### RAM Tab
- Swappiness: 60
- zRAM: Enabled with LZ4 compression
- zRAM size: 50% of total RAM

#### I/O Tab
- Scheduler: BFQ
- Read-ahead: 2048KB

#### Thermal Tab
- Thermal throttling: Enabled (for safety)
- Thermal profile: Performance

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
- Check if VSync/FSync are enabled

### Touchscreen issues
- Check if touchscreen boost is enabled
- Verify input boost frequencies
- Check DTBO compatibility

### Battery drain
- Reduce boost frequencies
- Enable thermal throttling
- Reduce zRAM size
- Use conservative governor

## Building from Source

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

### GitHub Actions Build
1. Fork this repository
2. Go to **Actions** tab
3. Select **Build Alioth Ultra Kernel**
4. Click **Run workflow**
5. Select your options and run

## Advanced Tuning

### Maximum Performance Profile
```bash
# CPU
echo "performance" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo "performance" > /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor

# GPU
echo "performance" > /sys/class/kgsl/kgsl-3d0/devfreq/governor

# I/O
echo "bfq" > /sys/block/sda/queue/scheduler

# Boosts
echo 1 > /sys/kernel/alioth_perf/touchscreen_boost
echo 1 > /sys/kernel/alioth_perf/frame_boost
echo 1 > /sys/kernel/alioth_perf/vsync_enabled
echo 1 > /sys/kernel/alioth_perf/fsync_enabled

# RAM
echo 1 > /sys/kernel/alioth_ram/adaptive_swappiness
echo 60 > /sys/kernel/alioth_ram/base_swappiness
```

### Maximum Battery Life Profile
```bash
# CPU
echo "schedutil" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo "schedutil" > /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor

# GPU
echo "msm-adreno-tz" > /sys/class/kgsl/kgsl-3d0/devfreq/governor

# I/O
echo "cfq" > /sys/block/sda/queue/scheduler

# Boosts
echo 0 > /sys/kernel/alioth_perf/touchscreen_boost
echo 0 > /sys/kernel/alioth_perf/frame_boost

# RAM
echo 0 > /sys/kernel/alioth_ram/adaptive_swappiness
echo 40 > /sys/kernel/alioth_ram/base_swappiness
```

### Gaming Profile
```bash
# CPU
echo "performance" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo "performance" > /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor

# GPU
echo "performance" > /sys/class/kgsl/kgsl-3d0/devfreq/governor
echo 900000000 > /sys/kernel/alioth_gpu/gpu_boost_freq

# Disable throttling
echo 0 > /sys/kernel/alioth_gpu/gpu_throttle_enable

# RAM
echo 1 > /sys/kernel/alioth_ram/adaptive_swappiness
echo 80 > /sys/kernel/alioth_ram/base_swappiness
```

## Credits

- **Base:** [AstideLabs/android_kernel_xiaomi_sm8250](https://github.com/AstideLabs/android_kernel_xiaomi_sm8250)
- **KernelSU:** [tiann/KernelSU](https://github.com/tiann/KernelSU)
- **SukiSU Ultra:** [SukiSU-Ultra/SukiSU-Ultra](https://github.com/SukiSU-Ultra/SukiSU-Ultra)
- **ReSukiSU:** [AstideLabs/ReSukiSU](https://github.com/AstideLabs/ReSukiSU)
- **ZyC-Clang:** [ZyCromerZ/Clang](https://github.com/ZyCromerZ/Clang)
- **LineageOS:** LineageOS Team
- **UtsavBalar1231:** For MIUI/HyperOS drivers

## License

This kernel is licensed under GPL v2. See LICENSE file for details.

**Disclaimer:** Flash at your own risk. The developers are not responsible for any damage to your device.
