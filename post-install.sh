#!/system/bin/sh
# Alioth Ultra Kernel Post-Install Script
# Runs after kernel installation via AnyKernel3

LOG=/data/local/tmp/alioth_post_install.log

echo "[$(date)] Alioth Ultra Kernel Post-Install Started" > $LOG

# Create performance tuning directory
mkdir -p /data/local/tmp/alioth_perf

# Set default performance settings
echo "[$(date)] Setting default performance settings..." >> $LOG

# CPU Governor settings
echo "performance" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor 2>/dev/null
echo "performance" > /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor 2>/dev/null

# Input Boost
echo 1 > /sys/kernel/alioth_perf/touchscreen_boost 2>/dev/null
echo 1 > /sys/kernel/alioth_perf/frame_boost 2>/dev/null
echo 100 > /sys/kernel/alioth_perf/input_boost_duration_ms 2>/dev/null

# RAM Optimization
echo 1 > /sys/kernel/alioth_ram/adaptive_swappiness 2>/dev/null
echo 60 > /sys/kernel/alioth_ram/base_swappiness 2>/dev/null

# GPU Boost
echo 900000000 > /sys/kernel/alioth_gpu/gpu_boost_freq 2>/dev/null
echo 100 > /sys/kernel/alioth_gpu/gpu_boost_duration_ms 2>/dev/null

# I/O Scheduler
echo "bfq" > /sys/block/sda/queue/scheduler 2>/dev/null
echo "bfq" > /sys/block/sdb/queue/scheduler 2>/dev/null
echo "bfq" > /sys/block/sdc/queue/scheduler 2>/dev/null

# Read-ahead
echo 2048 > /sys/block/sda/queue/read_ahead_kb 2>/dev/null
echo 2048 > /sys/block/sdb/queue/read_ahead_kb 2>/dev/null
echo 2048 > /sys/block/sdc/queue/read_ahead_kb 2>/dev/null

# Disable debugging
echo 0 > /sys/kernel/debug/sched_debug 2>/dev/null

# GPU Governor
echo "performance" > /sys/class/kgsl/kgsl-3d0/devfreq/governor 2>/dev/null

# Thermal settings (if supported)
if [ -f /sys/class/thermal/thermal_message/sconfig ]; then
    echo 10 > /sys/class/thermal/thermal_message/sconfig 2>/dev/null
fi

# Set swappiness
echo 60 > /proc/sys/vm/swappiness 2>/dev/null

# Set dirty ratio
echo 15 > /proc/sys/vm/dirty_ratio 2>/dev/null
echo 5 > /proc/sys/vm/dirty_background_ratio 2>/dev/null

# Set cache pressure
echo 100 > /proc/sys/vm/vfs_cache_pressure 2>/dev/null

# Enable zRAM if available
if [ -f /sys/block/zram0/disksize ]; then
    echo "[$(date)] Configuring zRAM..." >> $LOG
    echo lz4 > /sys/block/zram0/comp_algorithm 2>/dev/null
    # Size will be set by init script based on total RAM
fi

# Create init.d support
if [ -d /system/etc/init.d ]; then
    echo "[$(date)] Init.d support detected" >> $LOG
fi

# Log completion
echo "[$(date)] Alioth Ultra Kernel Post-Install Complete" >> $LOG
echo "" >> $LOG
echo "Kernel Features:" >> $LOG
echo "  - Maximum UI Smoothness" >> $LOG
echo "  - CPU/GPU/RAM Boost" >> $LOG
echo "  - VSync/FSync Control" >> $LOG
echo "  - Input Boost Optimization" >> $LOG
echo "  - Adaptive RAM Management" >> $LOG
echo "  - Root Manager Support" >> $LOG
echo "" >> $LOG
echo "Use Franco Kernel Manager to tune settings" >> $LOG
