#!/bin/bash
# Convenient Flashing Script for ESP32-C3 Super Mini & TENSTAR T-Display

PORT=${2:-/dev/ttyACM0}
ROLE=${1:-station1}

if [ ! -e "$PORT" ]; then
    PORT="/dev/ttyUSB0"
fi

if [ ! -e "$PORT" ]; then
    echo "❌ Fehler: Kein ESP32 an /dev/ttyACM0 oder /dev/ttyUSB0 gefunden!"
    exit 1
fi

BIN_FILE="web/firmware/${ROLE}.bin"

if [ ! -f "$BIN_FILE" ]; then
    echo "❌ Fehler: Firmware-Datei $BIN_FILE nicht gefunden!"
    exit 1
fi

echo "🚀 Flashe $ROLE auf $PORT ..."
~/.local/bin/esptool --port "$PORT" --chip esp32c3 write_flash 0x0 "$BIN_FILE"

echo "✅ Fertig! $ROLE wurde erfolgreich auf $PORT geflasht."
