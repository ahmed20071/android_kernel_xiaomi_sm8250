#!/bin/bash
#==================================================
# Alioth (POCO F3) Ultra Performance Kernel Build Script
# Based on SM8250 platform
# Supports: Android 16 / HyperOS 3 / AOSP
#==================================================

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
DEVICE="${1:-alioth}"
KSU_TYPE="${2:-sukisu}"
BUILD_TYPE="${3:-release}"

# Toolchain
CLANG_VERSION="16.0.6-20260510"
CLANG_URL="https://github.com/ZyCromerZ/Clang/releases/download/${CLANG_VERSION}-release/Clang-${CLANG_VERSION}.tar.gz"
CLANG_DIR="$HOME/zyc-clang"

# Kernel source
KERNEL_DIR="$(pwd)"
OUT_DIR="$KERNEL_DIR/out"
BUILD_DIR="$OUT_DIR/build"

# Jobs
JOBS=$(nproc --all)

# Functions
print_header() {
    echo -e "${BLUE}"
    echo "=================================================="
    echo "  Alioth Ultra Performance Kernel Builder"
    echo "  Device: $DEVICE"
    echo "  KSU Type: $KSU_TYPE"
    echo "  Build Type: $BUILD_TYPE"
    echo "=================================================="
    echo -e "${NC}"
}

setup_toolchain() {
    echo -e "${YELLOW}[*] Setting up toolchain...${NC}"

    if [ ! -d "$CLANG_DIR/bin" ]; then
        echo -e "${YELLOW}[*] Downloading ZyC-Clang ${CLANG_VERSION}...${NC}"
        mkdir -p "$CLANG_DIR"
        cd "$CLANG_DIR"
        wget -q --show-progress "$CLANG_URL" -O clang.tar.gz
        tar -xzf clang.tar.gz --strip-components=1
        rm clang.tar.gz
        cd "$KERNEL_DIR"
    fi

    export PATH="$CLANG_DIR/bin:$PATH"
    export CC="clang"
    export CXX="clang++"
    export AR="llvm-ar"
    export NM="llvm-nm"
    export STRIP="llvm-strip"
    export OBJCOPY="llvm-objcopy"
    export OBJDUMP="llvm-objdump"
    export READELF="llvm-readelf"

    echo -e "${GREEN}[✓] Toolchain ready${NC}"
}

setup_ksu() {
    echo -e "${YELLOW}[*] Setting up root manager: $KSU_TYPE...${NC}"

    case "$KSU_TYPE" in
        "kernelsu"|"ksu")
            echo -e "${YELLOW}[*] Integrating KernelSU...${NC}"
            curl -LSs "https://raw.githubusercontent.com/tiann/KernelSU/main/kernel/setup.sh" | bash -
            ;;
        "sukisu"|"sukisu-ultra")
            echo -e "${YELLOW}[*] Integrating SukiSU Ultra...${NC}"
            curl -LSs "https://raw.githubusercontent.com/SukiSU-Ultra/SukiSU-Ultra/main/kernel/setup.sh" | bash -s main
            ;;
        "resukisu")
            echo -e "${YELLOW}[*] Integrating ReSukiSU...${NC}"
            curl -LSs "https://raw.githubusercontent.com/AstideLabs/ReSukiSU/main/kernel/setup.sh" | bash -s main
            ;;
        "magisk")
            echo -e "${YELLOW}[*] Magisk support enabled (via KernelSU compatibility)${NC}"
            ;;
        "none"|"")
            echo -e "${YELLOW}[*] No root manager integration${NC}"
            ;;
        *)
            echo -e "${RED}[!] Unknown KSU type: $KSU_TYPE${NC}"
            exit 1
            ;;
    esac

    echo -e "${GREEN}[✓] Root manager setup complete${NC}"
}

clean_build() {
    echo -e "${YELLOW}[*] Cleaning build directory...${NC}"
    make -C "$KERNEL_DIR" O="$BUILD_DIR" ARCH=arm64 clean
    make -C "$KERNEL_DIR" O="$BUILD_DIR" ARCH=arm64 mrproper
    echo -e "${GREEN}[✓] Clean complete${NC}"
}

build_kernel() {
    echo -e "${YELLOW}[*] Building kernel for $DEVICE...${NC}"

    mkdir -p "$BUILD_DIR"

    # Generate config
    make -C "$KERNEL_DIR" O="$BUILD_DIR" ARCH=arm64         CC="$CC" CXX="$CXX" AR="$AR" NM="$NM" STRIP="$STRIP"         OBJCOPY="$OBJCOPY" OBJDUMP="$OBJDUMP" READELF="$READELF"         CROSS_COMPILE="aarch64-linux-gnu-" CROSS_COMPILE_ARM32="arm-linux-gnueabi-"         ${DEVICE}_perf_defconfig

    # Optimize for performance
    scripts/config --file "$BUILD_DIR/.config"         -e CONFIG_CC_OPTIMIZE_FOR_PERFORMANCE         -d CONFIG_CC_OPTIMIZE_FOR_SIZE

    # Enable LTO for better performance
    scripts/config --file "$BUILD_DIR/.config"         -e CONFIG_LTO         -e CONFIG_LTO_CLANG         -e CONFIG_THINLTO

    # Update config
    make -C "$KERNEL_DIR" O="$BUILD_DIR" ARCH=arm64         CC="$CC" CXX="$CXX" AR="$AR" NM="$NM" STRIP="$STRIP"         OBJCOPY="$OBJCOPY" OBJDUMP="$OBJDUMP" READELF="$READELF"         CROSS_COMPILE="aarch64-linux-gnu-" CROSS_COMPILE_ARM32="arm-linux-gnueabi-"         olddefconfig

    # Build
    make -C "$KERNEL_DIR" O="$BUILD_DIR" ARCH=arm64         CC="$CC" CXX="$CXX" AR="$AR" NM="$NM" STRIP="$STRIP"         OBJCOPY="$OBJCOPY" OBJDUMP="$OBJDUMP" READELF="$READELF"         CROSS_COMPILE="aarch64-linux-gnu-" CROSS_COMPILE_ARM32="arm-linux-gnueabi-"         -j$JOBS Image.gz dtb.img dtbo.img 2>&1 | tee "$OUT_DIR/build.log"

    echo -e "${GREEN}[✓] Kernel build complete${NC}"
}

package_kernel() {
    echo -e "${YELLOW}[*] Packaging kernel...${NC}"

    local PKG_DIR="$OUT_DIR/package"
    mkdir -p "$PKG_DIR"

    # Copy kernel image
    cp "$BUILD_DIR/arch/arm64/boot/Image.gz" "$PKG_DIR/"
    cp "$BUILD_DIR/arch/arm64/boot/dtb.img" "$PKG_DIR/" 2>/dev/null || true
    cp "$BUILD_DIR/arch/arm64/boot/dtbo.img" "$PKG_DIR/" 2>/dev/null || true

    # Create AnyKernel3 package
    local AK_DIR="$PKG_DIR/anykernel"
    mkdir -p "$AK_DIR"

    cat > "$AK_DIR/anykernel.sh" << 'EOF'
# AnyKernel3 Ramdisk Mod Script
# osm0sis @ xda-developers

properties() { 
    kernel.string=Alioth Ultra Performance Kernel
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

write_boot;
EOF

    # Copy kernel to AnyKernel
    cp "$BUILD_DIR/arch/arm64/boot/Image.gz" "$AK_DIR/"

    # Create zip
    cd "$PKG_DIR"
    zip -r9 "$OUT_DIR/Alioth-Ultra-${DEVICE}-${KSU_TYPE}-$(date +%Y%m%d).zip" anykernel/

    echo -e "${GREEN}[✓] Package created: $OUT_DIR/Alioth-Ultra-${DEVICE}-${KSU_TYPE}-$(date +%Y%m%d).zip${NC}"
}

print_info() {
    echo -e "${GREEN}"
    echo "=================================================="
    echo "  Build Complete!"
    echo "=================================================="
    echo "  Device: $DEVICE"
    echo "  KSU Type: $KSU_TYPE"
    echo "  Output: $OUT_DIR"
    echo "=================================================="
    echo -e "${NC}"
}

# Main
main() {
    print_header
    setup_toolchain
    setup_ksu

    if [ "$BUILD_TYPE" == "clean" ]; then
        clean_build
    fi

    build_kernel
    package_kernel
    print_info
}

main "$@"
