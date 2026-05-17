#!/bin/bash
# Module signing script for Alioth Ultra Kernel

KERNEL_DIR="$(pwd)"
OUT_DIR="$KERNEL_DIR/out"
BUILD_DIR="$OUT_DIR/build"

# Generate signing key if not exists
if [ ! -f "$KERNEL_DIR/certs/signing_key.pem" ]; then
    echo "[*] Generating module signing key..."
    mkdir -p "$KERNEL_DIR/certs"
    openssl req -new -x509 -newkey rsa:4096 -keyout "$KERNEL_DIR/certs/signing_key.pem"         -out "$KERNEL_DIR/certs/signing_key.x509" -days 3650 -nodes         -subj "/CN=Alioth Ultra Kernel Module Signing Key"
fi

# Sign modules
echo "[*] Signing kernel modules..."
find "$BUILD_DIR" -name "*.ko" -type f | while read module; do
    "$KERNEL_DIR/scripts/sign-file" sha256         "$KERNEL_DIR/certs/signing_key.pem"         "$KERNEL_DIR/certs/signing_key.x509"         "$module"
    echo "  Signed: $(basename $module)"
done

echo "[*] Module signing complete"
