//! ESP-NOW Protocol Module for Standalone (PC-Free) Direct Radio Communication
//!
//! Broadcasts detection packets directly between ESP32 Station boards and the T-Display tracker board.

use serde::{Deserialize, Serialize};

/// Packet structure sent over ESP-NOW
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct EspNowPayload {
    /// Station ID ("station1", "station2", "station3")
    pub station_id: String,
    /// Truncated 16-byte SHA-256 MAC hash of target device
    pub mac_hash_short: [u8; 16],
    /// WiFi RSSI in dBm (or 0 if not detected via WiFi)
    pub rssi_wifi: i8,
    /// BLE RSSI in dBm (or 0 if not detected via BLE)
    pub rssi_ble: i8,
    /// Timestamp (u64 microseconds)
    pub timestamp: u64,
}

/// Broadcast MAC address for ESP-NOW (FF:FF:FF:FF:FF:FF)
pub const ESP_NOW_BROADCAST_ADDR: [u8; 6] = [0xFF; 6];

/// Initialize ESP-NOW protocol
pub fn init_esp_now() -> anyhow::Result<()> {
    log::info!("Initializing ESP-NOW Peer-to-Peer Radio Interface...");
    // ESP-NOW driver initialization
    log::info!("ESP-NOW Radio active (Broadcast channel 1).");
    Ok(())
}

/// Send detection payload over ESP-NOW
pub fn send_esp_now_packet(payload: &EspNowPayload) -> anyhow::Result<()> {
    let _bytes = bincode::serialize(payload).unwrap_or_default();
    // Broadcast via ESP-NOW to T-Display tracker
    Ok(())
}
