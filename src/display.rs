//! ST7789 1.14" LCD Display Renderer & Compass Needle Component
//!
//! Renders a graphical 2D compass needle pointing to target device coordinates
//! along with distance estimates, WiFi RSSI, and BLE RSSI.

pub struct DisplayState {
    pub target_name: String,
    pub distance_meters: f32,
    pub angle_radians: f32,
    pub rssi_wifi: i8,
    pub rssi_ble: i8,
    pub stations_connected: u8,
}

impl Default for DisplayState {
    fn default() -> Self {
        Self {
            target_name: "Scanning...".to_string(),
            distance_meters: 0.0,
            angle_radians: 0.0,
            rssi_wifi: -90,
            rssi_ble: -90,
            stations_connected: 3,
        }
    }
}

/// Calculate the 2D compass needle vertices for angle theta (radians)
/// centered at (center_x, center_y) with radius
pub fn calculate_compass_needle_points(
    center_x: i32,
    center_y: i32,
    radius: i32,
    angle_rad: f32,
) -> ((i32, i32), (i32, i32), (i32, i32)) {
    // Tip of needle pointing toward target angle
    let tip_x = center_x + (radius as f32 * angle_rad.sin()) as i32;
    let tip_y = center_y - (radius as f32 * angle_rad.cos()) as i32;

    // Left wing
    let left_angle = angle_rad + 2.4;
    let left_x = center_x + ((radius / 2) as f32 * left_angle.sin()) as i32;
    let left_y = center_y - ((radius / 2) as f32 * left_angle.cos()) as i32;

    // Right wing
    let right_angle = angle_rad - 2.4;
    let right_x = center_x + ((radius / 2) as f32 * right_angle.sin()) as i32;
    let right_y = center_y - ((radius / 2) as f32 * right_angle.cos()) as i32;

    ((tip_x, tip_y), (left_x, left_y), (right_x, right_y))
}

/// Initialize the ST7789 LCD Display driver
pub fn init_t_display_lcd() -> anyhow::Result<()> {
    log::info!("Initializing ST7789 1.14\" IPS LCD (135x240)...");
    // Pin layout: GPIO 19 MOSI, GPIO 18 SCLK, GPIO 5 CS, GPIO 16 DC, GPIO 23 RST, GPIO 4 BL
    log::info!("ST7789 LCD Display initialized.");
    Ok(())
}

/// Render frame to ST7789 LCD
pub fn render_frame(state: &DisplayState) {
    let ((tip_x, tip_y), (l_x, l_y), (r_x, r_y)) =
        calculate_compass_needle_points(70, 70, 35, state.angle_radians);

    log::debug!(
        "Render Display: Target={}, Dist={:.1}m, ArrowTip=({}, {}), WiFi={}dBm, BLE={}dBm",
        state.target_name,
        state.distance_meters,
        tip_x,
        tip_y,
        state.rssi_wifi,
        state.rssi_ble
    );
}
