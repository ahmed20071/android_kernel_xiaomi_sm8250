# Changelog

## v1.0.0 (Initial Release)
- Initial release of Alioth Ultra Performance Kernel
- Based on SM8250 platform
- Optimized for Android 16 / HyperOS 3 / AOSP
- Maximum UI smoothness focus
- Full Franco Kernel Manager compatibility
- Support for multiple root managers:
  - KernelSU
  - SukiSU Ultra
  - ReSukiSU
  - Magisk compatibility
- Custom performance modules:
  - alioth_perf (main performance module)
  - alioth_cpu (CPU control)
  - alioth_gpu (GPU control)
  - alioth_ram (RAM optimization)
- VSync/FSync toggle support
- Input boost optimization
- Frame boost for 60fps
- Adaptive RAM management
- Multiple compression algorithms for zRAM
- BFQ I/O scheduler default
- BBR/BBR2 TCP congestion control
- WireGuard support
- MultiPath TCP support
- DAMON memory optimization
- Backported BPF for Android 16

## Known Issues
- Built DTBO may have brightness issues (use stock DTBO)
- First boot may take 2-3 minutes
- Some thermal profiles may need adjustment

## Upcoming Features
- Per-app performance profiles
- Advanced thermal management
- Custom voltage control
- More governor options
- GPU overclocking support
