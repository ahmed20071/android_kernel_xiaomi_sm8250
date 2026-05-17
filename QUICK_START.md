# Alioth Ultra Kernel - Quick Start

## For Users (Flash Ready Kernel)

1. Download the latest release from GitHub
2. Boot into TWRP/OrangeFox recovery
3. Flash the zip file
4. Wipe cache/dalvik (recommended)
5. Reboot and enjoy!

## For Developers (Build from Source)

### Prerequisites
- Linux PC or VM
- 50GB free space
- Good internet connection

### Quick Build (5 minutes setup)
```bash
# 1. Clone repository
git clone https://github.com/YOUR_USERNAME/android_kernel_xiaomi_sm8250.git
cd android_kernel_xiaomi_sm8250

# 2. Run setup
bash setup.sh

# 3. Build
bash build.sh alioth sukisu

# 4. Find output in out/ directory
```

### GitHub Actions Build (No setup needed)
1. Fork repository
2. Go to Actions tab
3. Run "Build Alioth Ultra Kernel" workflow
4. Download artifact when complete

## Configuration

### Using Franco Kernel Manager
1. Install FKM from Play Store
2. Grant root access
3. All tunables are under "Custom" tab
4. Adjust settings as needed

### Manual Configuration (via Terminal)
```bash
# CPU Governor
echo "performance" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

# GPU Governor
echo "performance" > /sys/class/kgsl/kgsl-3d0/devfreq/governor

# I/O Scheduler
echo "bfq" > /sys/block/sda/queue/scheduler

# Enable boosts
echo 1 > /sys/kernel/alioth_perf/touchscreen_boost
echo 1 > /sys/kernel/alioth_perf/frame_boost
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Bootloop | Flash stock kernel backup |
| Black screen | Restore stock DTBO |
| Touch not working | Check DTBO compatibility |
| Performance issues | Check governor settings |
| Battery drain | Reduce boost frequencies |

## Support

- GitHub Issues: https://github.com/YOUR_USERNAME/android_kernel_xiaomi_sm8250/issues
- XDA Thread: (link TBD)
- Telegram: (link TBD)
