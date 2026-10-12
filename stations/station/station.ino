// ESP32-C3 Super Mini – Ortungsstation (station1 / station2 / station3)
//
// Erfasst WLAN-Geraete auf allen Kanaelen 1–13 (Probe Requests + Datenframes
// von Clients) und BLE-Advertisements. Die MAC-Adressen werden sofort per
// SHA-256 gehasht. Die Erkennungen werden zwischengespeichert und nur auf
// Kanal 1 per ESP-NOW an den Peilsender (T-Display / CYD) gesendet, weil der
// Peilsender fest auf Kanal 1 hoert.
//
// Paketformat (40 Byte) ist kompatibel zu tracker.bin und tracker_cyd.bin.
//
// Station-ID: wird beim Build ueber station_id.h gesetzt (1, 2 oder 3).

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <mbedtls/sha256.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "station_id.h"
#ifndef STATION_NUM
#define STATION_NUM 1
#endif

#define STR_(x) #x
#define STR(x) STR_(x)
static const char STATION_ID[] = "station" STR(STATION_NUM);

// ---------------------------------------------------------------------------
// ESP-NOW Paket (identisch zu v1.0.x, 40 Byte)
// ---------------------------------------------------------------------------
struct __attribute__((packed)) DetectionPacket {
  char station_id[16];
  uint8_t mac_hash[16];   // erste 16 Byte von SHA-256(MAC)
  int8_t rssi_wifi;       // < 0 wenn per WLAN erkannt, sonst 0
  int8_t rssi_ble;        // < 0 wenn per BLE erkannt, sonst 0
  int8_t rssi_tracker;    // RSSI des Peilsenders (tracker_ping), 0 = unbekannt
  uint8_t reserved;
  uint32_t timestamp_ms;
};
static_assert(sizeof(DetectionPacket) == 40, "Paketgroesse muss 40 Byte sein");

static const uint8_t BROADCAST_ADDR[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ---------------------------------------------------------------------------
// Kanalplan: Kanal 1 wird zwischen jedem Block angesteuert, damit
// gesammelte Erkennungen zeitnah beim Peilsender ankommen.
// ---------------------------------------------------------------------------
static const uint8_t CHANNEL_PLAN[] = {1, 2, 3, 4, 1, 5, 6, 7, 1, 8, 9, 10, 1, 11, 12, 13};
static const uint32_t DWELL_MS = 120;          // Verweildauer auf Kanal 2–13
static const uint32_t DWELL_HOME_MS = 200;     // Verweildauer auf Kanal 1 (inkl. Senden)
static const uint32_t HEARTBEAT_MS = 1500;     // spaetestens dann ein Lebenszeichen senden
static const uint32_t TRACKER_TIMEOUT_MS = 5000;

// ---------------------------------------------------------------------------
// Zwischenspeicher fuer Erkennungen (pro Hash staerkster WLAN- und BLE-Wert)
// ---------------------------------------------------------------------------
struct Detection {
  uint8_t hash[16];
  int8_t rssi_wifi;
  int8_t rssi_ble;
  bool used;
};

static const int MAX_DETECTIONS = 64;
static Detection detections[MAX_DETECTIONS];
static portMUX_TYPE detectionsMux = portMUX_INITIALIZER_UNLOCKED;

static volatile int8_t trackerRssi = 0;
static volatile uint32_t trackerLastSeen = 0;
static uint32_t lastSendMs = 0;

static BLEScan *bleScan = nullptr;

static void hashMac(const uint8_t mac[6], uint8_t out[16]) {
  uint8_t digest[32];
  mbedtls_sha256(mac, 6, digest, 0);
  memcpy(out, digest, 16);
}

static void addDetection(const uint8_t mac[6], int8_t rssi, bool isBle) {
  uint8_t hash[16];
  hashMac(mac, hash);

  portENTER_CRITICAL(&detectionsMux);
  int freeSlot = -1;
  for (int i = 0; i < MAX_DETECTIONS; i++) {
    if (!detections[i].used) {
      if (freeSlot < 0) freeSlot = i;
      continue;
    }
    if (memcmp(detections[i].hash, hash, 16) == 0) {
      int8_t &slot = isBle ? detections[i].rssi_ble : detections[i].rssi_wifi;
      if (slot == 0 || rssi > slot) slot = rssi;
      portEXIT_CRITICAL(&detectionsMux);
      return;
    }
  }
  if (freeSlot >= 0) {
    Detection &d = detections[freeSlot];
    memcpy(d.hash, hash, 16);
    d.rssi_wifi = isBle ? 0 : rssi;
    d.rssi_ble = isBle ? rssi : 0;
    d.used = true;
  }
  portEXIT_CRITICAL(&detectionsMux);
}

// ---------------------------------------------------------------------------
// WLAN: Promiscuous-Callback
// ---------------------------------------------------------------------------
static void IRAM_ATTR onPromiscuous(void *buf, wifi_promiscuous_pkt_type_t type) {
  const wifi_promiscuous_pkt_t *pkt = (const wifi_promiscuous_pkt_t *)buf;
  if (pkt->rx_ctrl.sig_len < 24) return;

  const uint8_t *frame = pkt->payload;
  const uint8_t fcType = (frame[0] >> 2) & 0x3;
  const uint8_t fcSubtype = frame[0] >> 4;
  const uint8_t ds = frame[1] & 0x3;  // Bit0 = ToDS, Bit1 = FromDS

  bool isClient = false;
  if (fcType == 0 && fcSubtype == 4) {
    isClient = true;                  // Probe Request (Geraet sucht WLAN)
  } else if (fcType == 2 && ds == 1) {
    isClient = true;                  // Datenframe Client -> Router (inkl. QoS/Null)
  }
  // Beacons, Probe Responses, Action-Frames (ESP-NOW!) und Router -> Client
  // werden ignoriert.
  if (!isClient) return;

  const uint8_t *addr2 = frame + 10;  // Sender-MAC
  if (addr2[0] & 0x01) return;        // Multicast-Absender ist ungueltig

  addDetection(addr2, (int8_t)pkt->rx_ctrl.rssi, false);
}

// ---------------------------------------------------------------------------
// ESP-NOW: tracker_ping vom Peilsender empfangen
// ---------------------------------------------------------------------------
static void onEspNowRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len < 12) return;
  if (memcmp(data, "tracker", 7) != 0) return;
  if (info && info->rx_ctrl) {
    trackerRssi = (int8_t)info->rx_ctrl->rssi;
    trackerLastSeen = millis();
  }
}

// ---------------------------------------------------------------------------
// BLE
// ---------------------------------------------------------------------------
class ScanCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    uint8_t mac[6];
    String addr = advertisedDevice.getAddress().toString().c_str();
    if (sscanf(addr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) != 6) {
      return;
    }
    addDetection(mac, (int8_t)advertisedDevice.getRSSI(), true);
  }
};

// ---------------------------------------------------------------------------
// Senden (nur auf Kanal 1 aufrufen)
// ---------------------------------------------------------------------------
static void sendPacket(const uint8_t hash[16], int8_t rssiWifi, int8_t rssiBle, int8_t tracker) {
  DetectionPacket pkt = {};
  strncpy(pkt.station_id, STATION_ID, sizeof(pkt.station_id) - 1);
  memcpy(pkt.mac_hash, hash, 16);
  pkt.rssi_wifi = rssiWifi;
  pkt.rssi_ble = rssiBle;
  pkt.rssi_tracker = tracker;
  pkt.timestamp_ms = millis();
  esp_now_send(BROADCAST_ADDR, (const uint8_t *)&pkt, sizeof(pkt));
  delay(3);  // ESP-NOW Sendepuffer nicht ueberlaufen lassen
}

static void flushDetections() {
  static Detection pending[MAX_DETECTIONS];
  int count = 0;

  portENTER_CRITICAL(&detectionsMux);
  for (int i = 0; i < MAX_DETECTIONS; i++) {
    if (detections[i].used) {
      pending[count++] = detections[i];
      detections[i].used = false;
    }
  }
  portEXIT_CRITICAL(&detectionsMux);

  int8_t tracker = 0;
  if (trackerLastSeen != 0 && millis() - trackerLastSeen < TRACKER_TIMEOUT_MS) {
    tracker = trackerRssi;
  }

  for (int i = 0; i < count; i++) {
    sendPacket(pending[i].hash, pending[i].rssi_wifi, pending[i].rssi_ble, tracker);
  }

  if (count > 0) {
    lastSendMs = millis();
  } else if (millis() - lastSendMs >= HEARTBEAT_MS) {
    // Lebenszeichen: haelt die Station am Peilsender als "aktiv".
    // RSSI -127 liegt unter der Ziel-Schwelle des Peilsenders und erzeugt
    // daher kein Phantom-Ziel.
    const uint8_t zero[16] = {0};
    sendPacket(zero, -127, 0, tracker);
    lastSendMs = millis();
  }
}

static void channelHopperTask(void *) {
  size_t idx = 0;
  for (;;) {
    const uint8_t ch = CHANNEL_PLAN[idx];
    idx = (idx + 1) % sizeof(CHANNEL_PLAN);
    esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);

    if (ch == 1) {
      const uint32_t start = millis();
      flushDetections();
      const uint32_t spent = millis() - start;
      delay(spent < DWELL_HOME_MS ? DWELL_HOME_MS - spent : 10);
    } else {
      delay(DWELL_MS);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() == ESP_OK) {
    esp_now_register_recv_cb(onEspNowRecv);
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, BROADCAST_ADDR, 6);
    peer.channel = 0;  // 0 = aktueller Kanal
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
    esp_now_add_peer(&peer);
  }

  wifi_promiscuous_filter_t filter = {};
  filter.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA;
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(onPromiscuous);
  esp_wifi_set_promiscuous(true);

  xTaskCreate(channelHopperTask, "channel_hopper", 4096, nullptr, 1, nullptr);

  BLEDevice::init("ESP32_Sniffer");
  bleScan = BLEDevice::getScan();
  bleScan->setAdvertisedDeviceCallbacks(new ScanCallbacks(), true);
  bleScan->setActiveScan(true);
  bleScan->setInterval(100);  // 62,5 ms
  bleScan->setWindow(50);     // 31,25 ms -> 50 % Funkzeit bleibt fuer WLAN

  Serial.printf("%s gestartet (WLAN Kanal 1-13 + BLE)\n", STATION_ID);
}

void loop() {
  if (bleScan) {
    bleScan->start(1, false);
    bleScan->clearResults();
  }
  delay(50);
}
