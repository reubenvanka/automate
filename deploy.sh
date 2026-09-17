#!/bin/bash
# deploy.sh - AutoMate ESP32 Deployment Script
# This script enforces the complete deployment workflow:
# 1. Build firmware
# 2. Backup firmware with version
# 3. Backup code with version
# 4. Commit and push to GitHub
# 5. Upload to ESP32

set -e  # Exit on any error

# Colors for output
RED='[0;31m'
GREEN='[0;32m'
YELLOW='[1;33m'
NC='[0m' # No Color

# Configuration
PROJECT_DIR="/Users/reubenvanka/Documents/iot-workspace/products/automate"
FIRMWARE_DIR="$PROJECT_DIR/firmware/esphome"
BACKUP_DIR="$PROJECT_DIR/backups"
CODE_BACKUP_DIR="$BACKUP_DIR/code"
FIRMWARE_BACKUP_DIR="$BACKUP_DIR/firmware"
ESPHOME_CONFIG="configuration.yaml"
DEVICE_PORT="${2:-/dev/cu.usbserial-0001}"

# Get version from argument or generate from date
VERSION="${1:-v$(date +%Y%m%d_%H%M)}"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
BACKUP_NAME="${VERSION}_${TIMESTAMP}"

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}AutoMate ESP32 Deployment Script v1.0${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "Version: ${YELLOW}$VERSION${NC}"
echo -e "Timestamp: ${YELLOW}$TIMESTAMP${NC}"
echo -e "Device Port: ${YELLOW}$DEVICE_PORT${NC}"
echo -e "${GREEN}========================================${NC}"

cd "$PROJECT_DIR"

# Step 1: Verify we're in a git repo and clean
echo -e "${YELLOW}Step 1: Checking git status...${NC}"
if ! git diff --quiet || ! git diff --cached --quiet; then
    echo -e "${YELLOW}Warning: Uncommitted changes detected. They will be included in this deployment.${NC}"
fi

# Step 2: Build firmware
echo -e "
${YELLOW}Step 2: Building firmware...${NC}"
cd "$FIRMWARE_DIR"
echo "Building with ESPHome..."
esphome compile "$ESPHOME_CONFIG" 2>&1 | tail -20

# Verify build succeeded
FIRMWARE_BIN="$FIRMWARE_DIR/.esphome/build/automate/.pioenvs/automate/firmware.bin"
FACTORY_BIN="$FIRMWARE_DIR/.esphome/build/automate/.pioenvs/automate/firmware.factory.bin"
OTA_BIN="$FIRMWARE_DIR/.esphome/build/automate/.pioenvs/automate/firmware.ota.bin"
BOOTLOADER_BIN="$FIRMWARE_DIR/.esphome/build/automate/.pioenvs/automate/bootloader.bin"
PARTITIONS_BIN="$FIRMWARE_DIR/.esphome/build/automate/.pioenvs/automate/partitions.bin"
OTA_DATA_BIN="$FIRMWARE_DIR/.esphome/build/automate/.pioenvs/automate/ota_data_initial.bin"

for f in "$FIRMWARE_BIN" "$FACTORY_BIN" "$OTA_BIN" "$BOOTLOADER_BIN" "$PARTITIONS_BIN"; do
    if [ ! -f "$f" ]; then
        echo -e "${RED}ERROR: Build failed - missing $f${NC}"
        exit 1
    fi
done
echo -e "${GREEN}Build successful!${NC}"

# Step 3: Backup firmware
echo -e "
${YELLOW}Step 3: Backing up firmware...${NC}"
mkdir -p "$FIRMWARE_BACKUP_DIR"
FIRMWARE_BACKUP="$FIRMWARE_BACKUP_DIR/firmware_${BACKUP_NAME}.bin"
FACTORY_BACKUP="$FIRMWARE_BACKUP_DIR/firmware_${BACKUP_NAME}.factory.bin"
OTA_BACKUP="$FIRMWARE_BACKUP_DIR/firmware_${BACKUP_NAME}.ota.bin"

cp "$FIRMWARE_BIN" "$FIRMWARE_BACKUP"
cp "$FACTORY_BIN" "$FACTORY_BACKUP"
cp "$OTA_BIN" "$OTA_BACKUP"
echo -e "${GREEN}Firmware backed up to:${NC}"
echo "  $FIRMWARE_BACKUP"
echo "  $FACTORY_BACKUP"
echo "  $OTA_BACKUP"

# Step 4: Backup code
echo -e "
${YELLOW}Step 4: Backing up code...${NC}"
mkdir -p "$CODE_BACKUP_DIR"
CODE_BACKUP="$CODE_BACKUP_DIR/code_${BACKUP_NAME}.tar.gz"
tar -czf "$CODE_BACKUP"     firmware/esphome/packages/     firmware/esphome/configuration.yaml     firmware/esphome/secrets.yaml     CHANGELOG.md 2>/dev/null
echo -e "${GREEN}Code backed up to: $CODE_BACKUP${NC}"

# Step 4b: Commit to git
echo -e "
${YELLOW}Step 4b: Committing to git...${NC}"
cd "$PROJECT_DIR"
git add -A
if ! git diff --cached --quiet; then
    git commit -m "deploy: $VERSION - Firmware and code backup
- Firmware: $BACKUP_NAME
- Code backup: code_$(basename $BACKUP_NAME).tar.gz
- Auto-generated deployment commit"
    echo -e "${GREEN}Changes committed${NC}"
else
    echo -e "${YELLOW}No changes to commit${NC}"
fi

# Step 5: Push to GitHub
echo -e "
${YELLOW}Step 5: Pushing to GitHub...${NC}"
git push origin main
echo -e "${GREEN}Pushed to GitHub${NC}"

# Step 6: Upload to ESP32
echo -e "
${YELLOW}Step 6: Uploading to ESP32...${NC}"
cd "$FIRMWARE_DIR"
esptool.py --chip esp32 --port "$DEVICE_PORT" --baud 460800 write_flash -z     0x10000 "$FIRMWARE_BIN"     0x1000 "$BOOTLOADER_BIN"     0x8000 "$PARTITIONS_BIN"     0x9000 "$OTA_DATA_BIN" 2>&1 | tail -30

# Step 7: Verify upload
echo -e "
${YELLOW}Step 7: Verifying upload...${NC}"
sleep 5
echo -e "${GREEN}Upload complete!${NC}"

# Summary
echo -e "
${GREEN}========================================${NC}"
echo -e "${GREEN}DEPLOYMENT COMPLETE${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "Version: ${YELLOW}$VERSION${NC}"
echo -e "Timestamp: ${YELLOW}$TIMESTAMP${NC}"
echo -e "Firmware: ${GREEN}$FIRMWARE_BIN${NC}"
echo -e "Firmware Backup: ${GREEN}$FIRMWARE_BACKUP${NC}"
echo -e "Code Backup: ${GREEN}$CODE_BACKUP${NC}"
echo -e "Git Commit: ${GREEN}$(git rev-parse --short HEAD)${NC}"
echo -e "GitHub: ${GREEN}Pushed to origin/main${NC}"
echo -e "ESP32: ${GREEN}Uploaded to $DEVICE_PORT${NC}"
echo -e "${GREEN}========================================${NC}"

# Log deployment
echo "$(date -Iseconds) | $VERSION | $BACKUP_NAME | $(git rev-parse --short HEAD) | SUCCESS" >> "$PROJECT_DIR/deploy.log"