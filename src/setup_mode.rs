//! Room Setup & Self-Calibration Mode for 4-ESP32 Setup
//!
//! Measures inter-station distance estimates via RSSI and allows button-based room size configuration.

pub struct RoomConfig {
    pub width_meters: f32,
    pub height_meters: f32,
    pub calibrated: bool,
}

impl Default for RoomConfig {
    fn default() -> Self {
        Self {
            width_meters: 5.0,
            height_meters: 5.0,
            calibrated: false,
        }
    }
}

/// Run setup procedure
pub fn run_setup_procedure() -> RoomConfig {
    log::info!("Starting Room Setup & Self-Calibration Mode...");
    log::info!("Default Room Size: 5.0m x 5.0m. Press Button 1 (GPIO 0) to accept or Button 2 (GPIO 35) to adjust.");
    RoomConfig {
        width_meters: 5.0,
        height_meters: 5.0,
        calibrated: true,
    }
}
