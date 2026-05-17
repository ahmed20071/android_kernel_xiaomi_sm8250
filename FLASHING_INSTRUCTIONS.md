# Alioth Ultra Performance Kernel - Flashing Instructions

## BEFORE YOU FLASH

1. **Backup your current kernel**
   - The installer will automatically backup your stock kernel to /data/alioth_stock_kernel.img
   - You can restore it by flashing this file in recovery

2. **Check your ROM type**
   - MIUI/HyperOS users: Use the MIUI variant
   - AOSP/LineageOS users: Use the AOSP variant
   - Using the wrong variant may cause issues

3. **DTBO Warning**
   - Keep your stock DTBO for best compatibility
   - The built DTBO may have brightness issues
   - If you experience brightness problems, restore stock DTBO

## FLASHING STEPS

### Method 1: Custom Recovery (Recommended)
1. Boot into TWRP/OrangeFox recovery
2. Go to Install
3. Select the Alioth-Ultra-*.zip file
4. Swipe to flash
5. Wipe cache/dalvik (optional but recommended)
6. Reboot system

### Method 2: Fastboot (Advanced users)
1. Extract the zip file
2. Boot into fastboot mode
3. Flash kernel:
   ```
   fastboot flash boot Image.gz
   ```
4. Flash DTB (if needed):
   ```
   fastboot flash dtb dtb.img
   ```
5. Reboot:
   ```
   fastboot reboot
   ```

## POST-FLASH

1. **First boot may take 2-3 minutes** - This is normal
2. The kernel will automatically apply performance settings
3. Use Franco Kernel Manager to fine-tune settings
4. Check /data/local/tmp/alioth_perf.log for boot logs

## TROUBLESHOOTING

### Bootloop
- Boot to recovery
- Flash stock kernel backup
- Or flash stock ROM kernel

### Black screen
- Restore stock DTBO
- Flash correct variant (MIUI/AOSP)

### Performance issues
- Check if correct governor is set
- Verify boost settings
- Check thermal throttling

### Touchscreen not working
- Restore stock DTBO
- Check if touchscreen boost is enabled

## RESTORING STOCK KERNEL

1. Boot to recovery
2. Flash /data/alioth_stock_kernel.img (if backup exists)
3. Or flash stock ROM zip
4. Wipe cache/dalvik
5. Reboot

## SUPPORT

For support, visit:
- XDA Developers thread (link TBD)
- GitHub Issues: https://github.com/YOUR_USERNAME/android_kernel_xiaomi_sm8250/issues

## DISCLAIMER

Flash at your own risk. The developers are not responsible for any damage to your device.
