# Alioth Ultra Kernel - GitHub Setup Guide

## Step 1: Fork the Base Repository

1. Go to https://github.com/AstideLabs/android_kernel_xiaomi_sm8250
2. Click "Fork" button
3. Wait for the fork to complete

## Step 2: Clone Your Fork

```bash
git clone https://github.com/YOUR_USERNAME/android_kernel_xiaomi_sm8250.git
cd android_kernel_xiaomi_sm8250
```

## Step 3: Add Alioth Ultra Files

Copy all files from this package to your repository:

```bash
# Copy defconfigs
cp -r alioth_kernel/arch/arm64/configs/* arch/arm64/configs/

# Copy performance drivers
mkdir -p drivers/performance
cp -r alioth_kernel/drivers/performance/* drivers/performance/

# Copy device tree files
mkdir -p arch/arm64/boot/dts/qcom
cp -r alioth_kernel/arch/arm64/boot/dts/qcom/* arch/arm64/boot/dts/qcom/

# Copy build scripts
cp alioth_kernel/build.sh .
cp alioth_kernel/setup.sh .
cp alioth_kernel/merge_defconfig.sh .
cp alioth_kernel/sign-modules.sh .
cp alioth_kernel/load-modules.sh .

# Copy AnyKernel3 files
mkdir -p anykernel
cp -r alioth_kernel/anykernel/* anykernel/

# Copy GitHub Actions
mkdir -p .github/workflows
cp alioth_kernel/.github/workflows/* .github/workflows/

# Copy documentation
cp alioth_kernel/README.md .
cp alioth_kernel/DOCUMENTATION.md .
cp alioth_kernel/FLASHING_INSTRUCTIONS.md .
cp alioth_kernel/CHANGELOG.md .
cp alioth_kernel/LICENSE .
cp alioth_kernel/.gitignore .
cp alioth_kernel/build.config .
```

## Step 4: Integrate Performance Drivers

Add to `drivers/Makefile`:
```makefile
obj-y += performance/
```

Add to `drivers/Kconfig`:
```
source "drivers/performance/Kconfig"
```

## Step 5: Commit and Push

```bash
git add -A
git commit -m "Add Alioth Ultra Performance Kernel support

- Add performance drivers (alioth_perf, alioth_cpu, alioth_gpu, alioth_ram)
- Add optimized defconfigs for alioth
- Add device tree configurations
- Add build scripts and GitHub Actions
- Add AnyKernel3 packaging support
- Add comprehensive documentation"

git push origin main
```

## Step 6: Enable GitHub Actions

1. Go to your repository on GitHub
2. Click "Settings" tab
3. Click "Actions" in the left sidebar
4. Select "Allow all actions and reusable workflows"
5. Click "Save"

## Step 7: Build Your First Kernel

### Option 1: GitHub Actions (Recommended)
1. Go to "Actions" tab
2. Select "Build Alioth Ultra Kernel"
3. Click "Run workflow"
4. Select your options:
   - Device: alioth
   - Root Manager: sukisu (or your preference)
   - Build Type: release
5. Click "Run workflow"
6. Wait for build to complete (10-30 minutes)
7. Download the artifact from the workflow run

### Option 2: Local Build
```bash
# Install dependencies
sudo apt update
sudo apt install -y build-essential git curl wget bison flex zip bc cpio     libssl-dev ccache tar python3 python-is-python3 libncurses5-dev

# Build
bash build.sh alioth sukisu

# Find output in out/ directory
```

## Step 8: Create a Release

1. Go to "Releases" section
2. Click "Create a new release"
3. Tag version: v1.0.0
4. Release title: Alioth Ultra v1.0.0
5. Copy changelog from CHANGELOG.md
6. Upload the zip file from GitHub Actions artifact
7. Click "Publish release"

## Step 9: Share Your Kernel

- Create XDA Developers thread
- Share on Telegram/Discord groups
- Add to custom ROM threads

## Maintenance

### Update Base Kernel
```bash
# Add upstream remote
git remote add upstream https://github.com/AstideLabs/android_kernel_xiaomi_sm8250.git

# Fetch updates
git fetch upstream

# Merge updates
git merge upstream/main

# Resolve conflicts if any
# Push updates
git push origin main
```

### Update Root Managers
Edit `build.sh` and update the URLs in the `setup_ksu()` function.

### Update Toolchain
Edit `build.sh` and update `CLANG_VERSION` and `CLANG_URL`.

## Support

For issues or questions:
- GitHub Issues: https://github.com/YOUR_USERNAME/android_kernel_xiaomi_sm8250/issues
- XDA Developers (link TBD)
