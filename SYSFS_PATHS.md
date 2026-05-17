# Alioth Ultra Kernel - Sysfs Paths Reference

## CPU Control (/sys/kernel/alioth_cpu/)

| Path | Type | Description | Default |
|------|------|-------------|---------|
| little_governor | rw | Governor for little cores | schedutil |
| big_governor | rw | Governor for big cores | schedutil |
| prime_governor | rw | Governor for prime core | schedutil |
| little_min_freq | rw | Min freq for little cores (KHz) | 300000 |
| little_max_freq | rw | Max freq for little cores (KHz) | 1804800 |
| big_min_freq | rw | Min freq for big cores (KHz) | 710400 |
| big_max_freq | rw | Max freq for big cores (KHz) | 2400000 |
| prime_min_freq | rw | Min freq for prime core (KHz) | 710400 |
| prime_max_freq | rw | Max freq for prime core (KHz) | 2841600 |
| cpu_boost_freq_little | rw | Boost freq for little (KHz) | 1804800 |
| cpu_boost_freq_big | rw | Boost freq for big (KHz) | 2208000 |
| cpu_boost_freq_prime | rw | Boost freq for prime (KHz) | 2419200 |
| cpu_boost_duration_ms | rw | CPU boost duration (ms) | 100 |
| schedutil_up_rate_limit_us | rw | Schedutil up rate limit | 0 |
| schedutil_down_rate_limit_us | rw | Schedutil down rate limit | 0 |
| schedutil_hispeed_freq | rw | Schedutil hispeed freq | 0 |
| schedutil_hispeed_load | rw | Schedutil hispeed load | 90 |
| interactive_above_hispeed_delay | rw | Interactive above delay | 20000 |
| interactive_go_hispeed_load | rw | Interactive hispeed load | 90 |
| interactive_timer_rate | rw | Interactive timer rate | 20000 |

## GPU Control (/sys/kernel/alioth_gpu/)

| Path | Type | Description | Default |
|------|------|-------------|---------|
| gpu_boost_freq | rw | GPU boost frequency (Hz) | 900000000 |
| gpu_boost_duration_ms | rw | GPU boost duration (ms) | 100 |
| gpu_min_freq | rw | Min GPU frequency (Hz) | 220000000 |
| gpu_max_freq | rw | Max GPU frequency (Hz) | 900000000 |
| gpu_freq_table | r | Available GPU frequencies | - |
| gpu_default_pwrlevel | rw | Default power level | 5 |
| gpu_throttle_enable | rw | Enable throttling | 1 |
| gpu_bus_split | rw | Bus split | 1 |
| gpu_idle_timer | rw | Idle timer | 80 |
| gpu_governor | rw | GPU governor | msm-adreno-tz |
| gpu_preemption_preemptlevel | rw | Preemption level | 1 |

## RAM Control (/sys/kernel/alioth_ram/)

| Path | Type | Description | Default |
|------|------|-------------|---------|
| adaptive_swappiness | rw | Adaptive swappiness | 1 |
| base_swappiness | rw | Base swappiness | 60 |
| zram_disksize_pct | rw | zRAM size (% of RAM) | 50 |
| zram_comp_algorithm | rw | Compression algorithm | lz4 |
| min_free_kbytes | rw | Min free memory (KB) | 16384 |
| vfs_cache_pressure | rw | Cache pressure | 100 |
| dirty_ratio | rw | Dirty ratio | 15 |
| dirty_background_ratio | rw | Dirty background ratio | 5 |

## Performance Control (/sys/kernel/alioth_perf/)

| Path | Type | Description | Default |
|------|------|-------------|---------|
| input_boost_duration_ms | rw | Input boost duration | 100 |
| input_boost_freq_little | rw | Input boost freq little | 1804800 |
| input_boost_freq_big | rw | Input boost freq big | 2208000 |
| input_boost_freq_prime | rw | Input boost freq prime | 2419200 |
| cpu_boost_duration_ms | rw | CPU boost duration | 100 |
| gpu_boost_freq | rw | GPU boost frequency | 900000000 |
| gpu_boost_duration_ms | rw | GPU boost duration | 100 |
| ram_boost_swappiness | rw | RAM boost swappiness | 60 |
| vsync_enabled | rw | Enable VSync | 1 |
| fsync_enabled | rw | Enable FSync | 1 |
| touchscreen_boost | rw | Touchscreen boost | 1 |
| frame_boost | rw | Frame boost | 1 |

## Standard Linux Sysfs Paths

### CPU Governors
- /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
- /sys/devices/system/cpu/cpu*/cpufreq/scaling_min_freq
- /sys/devices/system/cpu/cpu*/cpufreq/scaling_max_freq

### GPU
- /sys/class/kgsl/kgsl-3d0/devfreq/governor
- /sys/class/kgsl/kgsl-3d0/max_gpuclk
- /sys/class/kgsl/kgsl-3d0/min_pwrlevel

### I/O Scheduler
- /sys/block/sda/queue/scheduler
- /sys/block/sdb/queue/scheduler
- /sys/block/sdc/queue/scheduler
- /sys/block/sda/queue/read_ahead_kb

### Memory
- /proc/sys/vm/swappiness
- /proc/sys/vm/dirty_ratio
- /proc/sys/vm/dirty_background_ratio
- /proc/sys/vm/vfs_cache_pressure

### Thermal
- /sys/class/thermal/thermal_zone*/temp
- /sys/class/thermal/thermal_message/sconfig
