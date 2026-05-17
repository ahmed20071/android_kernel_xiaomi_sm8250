#!/bin/bash
# Merge Alioth defconfigs

cat arch/arm64/configs/alioth_perf_defconfig arch/arm64/configs/alioth_addons_defconfig > arch/arm64/configs/alioth_merged_defconfig
echo "Defconfigs merged to alioth_merged_defconfig"
