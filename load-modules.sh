#!/system/bin/sh
# Alioth Ultra Kernel Module Loader
# Loads custom performance modules on boot

LOG=/data/local/tmp/alioth_modules.log

echo "[$(date)] Loading Alioth Ultra modules..." > $LOG

# Load performance modules
if [ -f /system/lib/modules/alioth_perf.ko ]; then
    insmod /system/lib/modules/alioth_perf.ko 2>/dev/null
    echo "[$(date)] Loaded: alioth_perf.ko" >> $LOG
fi

if [ -f /system/lib/modules/alioth_cpu.ko ]; then
    insmod /system/lib/modules/alioth_cpu.ko 2>/dev/null
    echo "[$(date)] Loaded: alioth_cpu.ko" >> $LOG
fi

if [ -f /system/lib/modules/alioth_gpu.ko ]; then
    insmod /system/lib/modules/alioth_gpu.ko 2>/dev/null
    echo "[$(date)] Loaded: alioth_gpu.ko" >> $LOG
fi

if [ -f /system/lib/modules/alioth_ram.ko ]; then
    insmod /system/lib/modules/alioth_ram.ko 2>/dev/null
    echo "[$(date)] Loaded: alioth_ram.ko" >> $LOG
fi

echo "[$(date)] Module loading complete" >> $LOG
