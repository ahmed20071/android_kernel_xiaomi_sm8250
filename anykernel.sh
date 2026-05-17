# AnyKernel3 Ramdisk Mod Script
# osm0sis @ xda-developers

properties() { 
    kernel.string=Alioth Ultra Performance Kernel by AliothDev
    do.devicecheck=1
    do.modules=0
    do.systemless=1
    do.cleanup=1
    do.cleanuponabort=0
    device.name1=alioth
    device.name2=aliothin
    device.name3=thyme
    supported.versions=11 - 16
    supported.patchlevels=2024-01 - 2026-12
}

block=/dev/block/bootdevice/by-name/boot;
is_slot_device=0;
ramdisk_compression=auto;

set_perm_recursive 0 0 755 644 $ramdisk/*;
set_perm_recursive 0 0 750 750 $ramdisk/init* $ramdisk/sbin;

dump_boot;

# Add init script for performance tuning
if [ -f $home/init.alioth_perf.sh ]; then
    ui_print "Installing Alioth performance init script..."
    cp $home/init.alioth_perf.sh /system/bin/init.alioth_perf.sh
    chmod 0755 /system/bin/init.alioth_perf.sh

    # Add to init.rc if possible
    if [ -f $ramdisk/init.rc ]; then
        if ! grep -q "init.alioth_perf.sh" $ramdisk/init.rc; then
            echo "    exec_background u:r:su:s0 root root -- /system/bin/init.alioth_perf.sh" >> $ramdisk/init.rc
        fi
    fi
fi

# Backup original kernel if not exists
if [ ! -f /data/alioth_stock_kernel.img ]; then
    ui_print "Backing up stock kernel..."
    dd if=$block of=/data/alioth_stock_kernel.img bs=4096
fi

write_boot;

ui_print ""
ui_print "========================================"
ui_print "  Alioth Ultra Performance Kernel"
ui_print "  Installed Successfully!"
ui_print "========================================"
ui_print ""
ui_print "Features:"
ui_print "  - Maximum UI Smoothness"
ui_print "  - CPU/GPU/RAM Boost"
ui_print "  - VSync/FSync Control"
ui_print "  - Input Boost Optimization"
ui_print "  - Adaptive RAM Management"
ui_print "  - Root Manager Support"
ui_print ""
ui_print "Use Franco Kernel Manager to tune"
ui_print "========================================"
