#!/bin/bash
#==================================================
# Alioth Ultra Kernel Repository Setup Script
#==================================================

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}"
echo "=================================================="
echo "  Alioth Ultra Kernel Repository Setup"
echo "=================================================="
echo -e "${NC}"

# Check if we're in a kernel source directory
if [ ! -d "arch/arm64" ]; then
    echo -e "${RED}[!] Error: This doesn't appear to be a kernel source directory${NC}"
    echo -e "${YELLOW}[*] Please run this script from the root of your kernel source${NC}"
    exit 1
fi

KERNEL_DIR=$(pwd)

echo -e "${YELLOW}[*] Setting up Alioth Ultra Performance Kernel...${NC}"

# Create performance drivers directory
mkdir -p drivers/performance

# Copy performance driver files
echo -e "${YELLOW}[*] Installing performance drivers...${NC}"
cp -r arch/arm64/configs/alioth_perf_defconfig arch/arm64/configs/ 2>/dev/null || true

# Add performance drivers to main Makefile
if ! grep -q "drivers/performance/Makefile" drivers/Makefile; then
    echo -e "${YELLOW}[*] Adding performance drivers to build system...${NC}"
    echo "obj-y += performance/" >> drivers/Makefile
fi

# Add performance drivers Kconfig
if ! grep -q "drivers/performance/Kconfig" drivers/Kconfig; then
    echo -e "${YELLOW}[*] Adding performance drivers Kconfig...${NC}"
    echo "source "drivers/performance/Kconfig"" >> drivers/Kconfig
fi

# Copy device tree
mkdir -p arch/arm64/boot/dts/qcom
cp arch/arm64/boot/dts/qcom/kona-alioth.dts arch/arm64/boot/dts/qcom/ 2>/dev/null || true

# Set up AnyKernel3
if [ ! -d "anykernel" ]; then
    echo -e "${YELLOW}[*] Setting up AnyKernel3 packaging...${NC}"
    mkdir -p anykernel
fi

# Create build symlink
if [ ! -f "build.sh" ]; then
    echo -e "${YELLOW}[*] Creating build script symlink...${NC}"
    ln -sf scripts/build.sh build.sh 2>/dev/null || true
fi

echo -e "${GREEN}[✓] Repository setup complete!${NC}"
echo ""
echo -e "${BLUE}Next steps:${NC}"
echo "  1. Copy the files from this package to your kernel source"
echo "  2. Run: bash build.sh alioth sukisu"
echo "  3. Or use GitHub Actions for automated builds"
echo ""
