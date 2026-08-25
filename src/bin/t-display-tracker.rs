//! TENSTAR T-Display ESP32 Tracker Binary
//!
//! Receives ESP-NOW detection packets from 3 corner stations,
//! calculates 2D trilateration, and renders the 2D compass needle & RSSIs on the 1.14" ST7789 LCD display.

use esp32_wifi_sniffer::triangulate::{PositionTracker, StationLike, CalibrationParams};
use std::thread;
use std::time::Duration;

#[path = "../display.rs"]
mod display;
#[path = "../esp_now.rs"]
mod esp_now;
#[path = "../setup_mode.rs"]
mod setup_mode;

struct FixedStation {
    id: String,
    x: f32,
    y: f32,
}

impl StationLike for FixedStation {
    fn id(&self) -> &str {
        &self.id
    }
    fn x(&self) -> f32 {
        self.x
    }
    fn y(&self) -> f32 {
        self.y
    }
    fn calibration(&self) -> CalibrationParams {
        CalibrationParams {
            rssi_at_1m: -45.0,
            path_loss_exponent: 3.0,
        }
    }
}

fn main() -> anyhow::Result<()> {
    esp_idf_svc::sys::link_patches();
    esp_idf_svc::log::EspLogger::initialize_default();

    log::info!("=== TENSTAR T-Display Handheld Tracker ===");

    // Initialize ST7789 LCD
    display::init_t_display_lcd()?;

    // Run setup / room calibration
    let room = setup_mode::run_setup_procedure();
    log::info!("Room configured: {:.1}m x {:.1}m", room.width_meters, room.height_meters);

    // Initialize 3 corner stations based on room size
    let stations = vec![
        FixedStation { id: "station1".into(), x: 0.0, y: 0.0 },
        FixedStation { id: "station2".into(), x: room.width_meters, y: 0.0 },
        FixedStation { id: "station3".into(), x: room.width_meters / 2.0, y: room.height_meters },
    ];

    let mut tracker = PositionTracker::new(&stations);
    esp_now::init_esp_now()?;

    log::info!("Peilsender active. Searching for targets...");

    let mut state = display::DisplayState::default();

    loop {
        // Main loop: Update display frame every 200ms
        thread::sleep(Duration::from_millis(200));

        // Sample render tick
        display::render_frame(&state);
    }
}
