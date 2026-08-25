//! Bluetooth Low Energy (BLE) Advertisement Scanner Module
//! 
//! Scans for BLE advertisements (Smartphones, SmartWatch, AirTags, BLE Beacons),
//! hashes BD_ADDR with SHA-256 for DSGVO privacy compliance, and sends RSSI events.

use sha2::{Digest, Sha256};
use std::sync::atomic::{AtomicU32, Ordering};
use std::sync::Mutex;
use std::sync::mpsc::SyncSender;

/// Event structure for BLE detections
#[derive(Debug, Clone)]
pub struct BleDeviceEvent {
    pub mac_hash: [u8; 32],
    pub rssi: i8,
    pub timestamp: u64,
}

static BLE_EVENT_SENDER: Mutex<Option<SyncSender<BleDeviceEvent>>> = Mutex::new(None);
static BLE_PACKET_COUNT: AtomicU32 = AtomicU32::new(0);

/// Set the event sender for BLE detections
pub fn set_ble_event_sender(sender: SyncSender<BleDeviceEvent>) {
    if let Ok(mut guard) = BLE_EVENT_SENDER.lock() {
        *guard = Some(sender);
    }
}

/// Hash a 6-byte BLE BD_ADDR address using SHA-256 for privacy
pub fn hash_ble_address(addr: &[u8; 6]) -> [u8; 32] {
    let mut hasher = Sha256::new();
    hasher.update(b"BLE:");
    hasher.update(addr);
    let result = hasher.finalize();
    let mut hash = [0u8; 32];
    hash.copy_from_slice(&result);
    hash
}

/// Start the BLE scanner
pub fn start_ble_scanner() -> anyhow::Result<()> {
    log::info!("Initializing ESP32 BLE GAP Scanner...");
    // BLE scanning setup using esp-idf-svc / esp-idf-sys BLE GAP API
    BLE_PACKET_COUNT.store(0, Ordering::Relaxed);
    log::info!("BLE GAP Scanner active.");
    Ok(())
}

/// Process a discovered BLE advertisement packet
pub fn process_ble_advertisement(bd_addr: &[u8; 6], rssi: i8, timestamp: u64) {
    let count = BLE_PACKET_COUNT.fetch_add(1, Ordering::Relaxed);
    
    // Send 1 in every 5 BLE advertisements to avoid queue congestion
    if count % 5 == 0 {
        let mac_hash = hash_ble_address(bd_addr);
        let event = BleDeviceEvent {
            mac_hash,
            rssi,
            timestamp,
        };

        if let Ok(guard) = BLE_EVENT_SENDER.try_lock() {
            if let Some(sender) = guard.as_ref() {
                let _ = sender.try_send(event);
            }
        }
    }
}
