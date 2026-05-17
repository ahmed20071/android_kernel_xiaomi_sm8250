#!/system/bin/sh
# Alioth Ultra Performance Init Script
# Runs on boot to optimize system settings

# Log file
LOG=/data/local/tmp/alioth_perf.log

echo "[$(date)] Alioth Ultra Performance Init Started" > $LOG

# Wait for boot to complete
while [ "$(getprop sys.boot_completed)" != "1" ]; do
    sleep 1
done

sleep 30

echo "[$(date)] Boot complete, applying optimizations..." >> $LOG

# CPU Governor - Performance
echo "performance" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo "performance" > /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor

# Input Boost
echo 1 > /sys/kernel/alioth_perf/touchscreen_boost
echo 1 > /sys/kernel/alioth_perf/frame_boost
echo 100 > /sys/kernel/alioth_perf/input_boost_duration_ms

# RAM Optimization
echo 1 > /sys/kernel/alioth_ram/adaptive_swappiness
echo 60 > /sys/kernel/alioth_ram/base_swappiness

# I/O Scheduler
echo "bfq" > /sys/block/sda/queue/scheduler
echo "bfq" > /sys/block/sdb/queue/scheduler
echo "bfq" > /sys/block/sdc/queue/scheduler

# Read-ahead
echo 2048 > /sys/block/sda/queue/read_ahead_kb
echo 2048 > /sys/block/sdb/queue/read_ahead_kb
echo 2048 > /sys/block/sdc/queue/read_ahead_kb

# Disable debugging
echo 0 > /sys/kernel/debug/sched_debug

# Thermal (if supported)
if [ -f /sys/class/thermal/thermal_message/sconfig ]; then
    echo 10 > /sys/class/thermal/thermal_message/sconfig
fi

# GPU Governor
echo "performance" > /sys/class/kgsl/kgsl-3d0/devfreq/governor

# Memory reclaim
if [ -f /proc/sys/vm/drop_caches ]; then
    echo 3 > /proc/sys/vm/drop_caches
    sleep 1
    echo 0 > /proc/sys/vm/drop_caches
fi

echo "[$(date)] Alioth Ultra Performance Init Complete" >> $LOG
