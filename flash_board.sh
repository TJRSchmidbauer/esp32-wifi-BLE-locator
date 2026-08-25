#!/usr/bin/env bash

# Helper script to flash ESP32 WiFi & BLE Locator boards
PORT="/dev/ttyUSB0"
if [ ! -e "$PORT" ]; then
    PORT="/dev/ttyACM0"
fi

BOARD="$1"

if [ -z "$BOARD" ]; then
    echo "Verwendung: ./flash_board.sh [station1 | station2 | station3 | tracker]"
    exit 1
fi

if [ ! -f "firmware/${BOARD}.bin" ]; then
    echo "Fehler: Firmware firmware/${BOARD}.bin nicht gefunden!"
    exit 1
fi

echo "Flashe ${BOARD} auf Port ${PORT}..."
esptool --port "${PORT}" write_flash 0x0 "firmware/${BOARD}.bin"
echo "Fertig!"
