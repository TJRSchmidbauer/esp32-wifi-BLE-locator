# ESP32 Dual WiFi & BLE 90° Radar & Relativ-Ortungssystem (T-Display & CYD)

Ein autarkes, datenschutzkonformes Ortungssystem auf Basis von C++ (Arduino ESP32 Core) für den ESP32. Das System erfasst zeitgleich **WLAN Probe Requests & Datenframes auf Kanal 1** und **Bluetooth Low Energy (BLE) Advertisements** (Smartphones, Smartwatches, In-Ear-Headsets, AirTags oder BLE Beacons) und ermöglicht die **2D-Relativ-Ortung im Raum** (Messung der Entfernung direkt zu deinen Händen!) sowie die **Richtungsführung per 90° Sektor-Radar** auf einem **1.14" LilyGO / TENSTAR T-Display** – ganz **ohne PC, ohne Server und ohne WLAN-Router!**

---

### 💡 Verfügbare Peilsender im Repository

Beide Peilsender nutzen **dieselben 3 Stationen** (ESP32-C3 Super Mini) und werden über dieselbe Flash-Seite geflasht – du brauchst nur einen davon:

* 🎯 **T-Display Edition** (`tracker.bin`): Für das **1.14" LilyGO / TENSTAR T-Display ESP32** mit 90° Sektor-Radar und dynamischer Blickrichtungs-Führung.
* 📱 **CYD Edition** (`tracker_cyd.bin`): Für das **2.8" CYD Touch-Display (ESP32-2432S028)** mit 320x240 LCD, starrem 2D-Raumplan, interaktiver Touch-Geräteauswahl (`[NEXT]` / `[CALIB]`) und Touch-Kalibrierung (siehe [CYD-Anzeige](#-display-anzeige-auf-dem-cyd-28-320x240-ili9341-lcd)).

---

> 🤖 **Entwicklungs-Hinweis**: Dieses Repository und das gesamte C++ / WebSerial-System werden mit Unterstützung von **Google Antigravity** (einer fortschrittlichen agentischen KI für Pair-Programming von Google DeepMind) entwickelt, optimiert und gewartet.

---

## 🛒 Benötigte Hardware & 3D-Druck Gehäuse

| Komponente | Rolle im System | Produkt-Link (AliExpress) | 🖨️ 3D-Druck Gehäuse (MakerWorld) |
| :--- | :--- | :--- | :--- |
| **TENSTAR / LilyGO T-Display ESP32** | Handheld Peilsender mit 1.14" IPS LCD Display | 🔗 [AliExpress T-Display kaufen](https://de.aliexpress.com/item/1005005970553639.html?gatewayAdapt=glo2deu) | 📦 [LilyGO T-Display 1.14" Case (MakerWorld)](https://makerworld.com/de/models/1921314-lilygo-t-display-1-14-case#profileId-2061606) |
| **ESP32 CYD (ESP32-2432S028)** *(alternativ zum T-Display)* | Handheld 2.8" ILI9341 Touch-Display | 🔗 [AliExpress CYD Display kaufen](https://de.aliexpress.com/item/1005006599448997.html) | 📦 [ESP32 CYD 2.8" Snap-Fit Case (MakerWorld)](https://makerworld.com/de/models/3180623) |
| **3x ESP32-C3 Super Mini** | Ecken-Stationen (WiFi Kanal 1 + BLE Sniffer) | 🔗 [AliExpress ESP32-C3 Super Mini kaufen](https://de.aliexpress.com/item/1005006599448997.html?spm=a2g0o.order_list.order_list_main.55.514f5c5fRjT0iT&gatewayAdapt=glo2deu) | 📦 [SuperMini Snap-Fit Case (MakerWorld)](https://makerworld.com/de/models/2851590-esp32-s3-supermini-case-snap-fit-options#profileId-3180623) |

---

## 📸 System-Übersicht & Raum-Aufbau

```
                         Station 2 (Vorne / Mitte)
                                  [ 📡 ]
                                    ▲
                                   / \
                                  /   \
                                 /     \
                                /   📱  \  ◄── Gesuchtes Zielgerät
                               /   (•)   \     (wird trianguliert)
                              /           \
                             /   ┌─────┐   \
                            /    │T-Dis│    \  ◄── Du mit dem T-Display
                           /     └─────┘     \     (Blickrichtung zu Station 2)
                          /                   \
                         /                     \
                        ▼                       ▼
                     [ 📡 ]                   [ 📡 ]
                Station 1 (Links)        Station 3 (Rechts)
```

---

## 📏 Empfohlene Raumgrößen & Ortungsgenauigkeit

| Raumgröße | Abstand der Stationen | Ortungsverhalten & Präzision | Bewertung |
| :--- | :--- | :--- | :--- |
| **Unter 2m × 2m** *(< 4 m²)* | $< 1,5\text{ m}$ | ⚠️ **Zu klein**: Signale überlagern sich. Das Radar zeigt das Ziel meist nur starr in der Mitte an. | ❌ Nicht empfohlen |
| **3m × 3m** *(9 m²)* | $\approx 2,5\text{ m}$ | 🟢 **Geringste sinnvolle Mindestgröße**: Triangulation funktioniert gut. Genauigkeit im Raum ca. $\pm 0,5\text{m}$ bis $\pm 0,8\text{m}$. | ✅ **Mindestgröße** |
| **4m × 5m** *(20 m²)* | $\approx 3,5\text{ m}$ | 🌟 **Optimaler Bereich (Standard-Zimmer)**: Perfekte Signaltrennung. Sehr gute "Heiß/Kalt"-Führung beim Annähern! | ⭐ **Ideal** |
| **6m × 8m** *(48 m²)* | $\approx 5,5\text{ m}$ | 🟢 **Sehr gut (Großer Raum / Büro)**: Volle Funktion bis ca. 10m Reichweite pro Station. | ✅ Sehr gut |

---

## ✨ Hauptmerkmale

* 📡 **Dual Triangulation (Ziel + Handheld)**: Trianguliert zeitgleich die Position des gesuchten Geräts und deines T-Display Handhelds im Raum.
* 🎓 **Raum-Kalibrierung per Taste 2**: Per Knopfdruck misst das T-Display 3s lang die Raumakustik und passt die Distanzberechnung an den Raum an.
* 📶 **Fester ESP-NOW-Kanal 1 + BLE-Scan mit 50 % Duty-Cycle**: Stationen und T-Display funken auf demselben Kanal, damit die Messwerte zuverlässig ankommen (siehe Hinweis unten).
* 📏 **"Heiß / Kalt"-Entfernung zu deinen Händen**: Die Zahl `DISTANZ ZU DIR` zählt live herunter (z. B. `3.2m ➔ 1.5m ➔ 0.4m`), wenn du dich auf das Versteck zubewegst!
* 🧭 **Blickrichtungs-Modell (Station 2)**: Halte das T-Display mit dem Blick nach vorne Richtung Station 2. Das 90°-Radar richtet sich perfekt auf deine Blickrichtung aus.
* 🛡️ **Paket-Wiederholungs-Filter (Hit-Count >= 3)**: Erfordert mindestens 3 empfangene Pakete und $\ge 2$ Stationen, um zufällige Signale von Autos oder Nachbarn zu 100% stummzuschalten.
* 📺 **Flimmerfreie Delta-Render-Engine**: Vermeidet zeilenweises SPI-Nachladen – das Bild steht absolut starr und gestochen scharf.
* 🔒 **Privacy-First (DSGVO-Konform)**: Eindeutige MAC-Adressen werden noch **direkt auf dem ESP32-Chip per SHA-256 gehasht**. Es werden keine Klardaten oder Paketinhalte übertragen oder gespeichert.

---

## 📐 Display-Anzeige auf dem TENSTAR T-Display (1.14" ST7789 LCD)

```
┌────────────────────────────────────────┐
│ ESP32 RADAR (BLICK ZU ST.2)            │
│                                        │
│     \  |  /      ZIEL: a591a6d4        │
│      \ | /       DIST: 1.8m  ◄── Hände │
│       (•)        WiFi: -68dB           │
│        ▲         BLE : -72dB           │
│ ────────────────────────────────────── │
│ [3/3 Stat. aktiv] | Ziel: 1            │
└────────────────────────────────────────┘
```

---

## 📐 Display-Anzeige auf dem CYD 2.8" (320x240 ILI9341 LCD)

```
┌──────────────────────────────────────┬─────────────────────────┐
│ ESP32 CYD  |  2D TOUCH LOCATOR       │ TARGET ID: a591a6d4     │
├──────────────────────────────────────┤ PROXIMITY: 1.8 m        │
│  [Station 2]                         │ WIFI:   -68 dB          │
│      📡                              │ BLE :   -72 dB          │
│       │                              ├─────────────────────────┤
│       │       🟢 (WiFi)              │ NODES:   3/3            │
│       │              🟣 (BLE)        │ DEVICES: 1              │
│       │   🟡 (Target)                ├─────────────────────────┤
│       │                              │ [  NEXT  ] (Touch)      │
│  📡───────────────📡                 │ [  CALIB ] (Touch)      │
│ [Station 1]   [Station 3]            │                         │
└──────────────────────────────────────┴─────────────────────────┘
```

* Tippen auf ein Blip wählt das Gerät aus, `[NEXT]` / `[CALIB]` steuern Zielwahl und Raumkalibrierung.
* Die störende RGB-LED auf der Rückseite des CYD wird abgeschaltet.

---

## 🛠️ Fix (v1.0.1 Stations-Firmware): T-Display zeigt „[0/3 Stat.]“

Der Peilsender (T-Display oder CYD) zählt eine Station nur als aktiv, wenn innerhalb von 5 s ein ESP-NOW-Paket von ihr ankommt. Er hört dabei fest auf **WLAN-Kanal 1**. Die Stationen der Version 1.0.0 haben ihre Pakete aber nur selten auf diesem Kanal gesendet:

1. **Channel Hopping 1–13 (150 ms je Kanal)** – ESP-NOW sendet immer auf dem aktuellen Kanal, nur ca. 1/13 der Pakete lag auf Kanal 1.
2. **BLE-Scan mit 97,5 % Duty-Cycle** (`setInterval(40)` / `setWindow(39)`) – der ESP32-C3 hat nur ein Funkteil, WLAN/ESP-NOW bekam kaum Sendezeit.
3. **Jedes empfangene WLAN-Frame wurde weitergeleitet**, auch Router-Beacons und die ESP-NOW-Pakete der anderen Stationen bzw. der `tracker_ping` – die Stationen haben sich so gegenseitig „hochgeschaukelt“.

Die Stations-Firmware `station1–3.bin` (v1.0.1) wurde deshalb gepatcht:

| Änderung | Vorher | Nachher |
| :--- | :--- | :--- |
| WLAN-Kanal | Hopping 1–13 | fest Kanal 1 (gleicher Kanal wie T-Display) |
| Weitergeleitete Frames | alle (inkl. Beacons, ESP-NOW) | nur Frame-Control-Byte < `0x80` (Probe Requests, Datenframes); Beacons/Action-Frames/ESP-NOW werden verworfen |
| BLE-Scan-Fenster | 39/40 (97,5 %) | 20/40 (50 %) |

Paketformat (40 Byte) und Peilsender-Firmware (T-Display & CYD) sind unverändert. **Alle drei Stationen neu flashen**, der Peilsender muss nicht neu geflasht werden.

> Hinweis für den Arduino-Sketch: Die gleichen Änderungen sollten auch im Quellcode der Stationen nachgezogen werden (`esp_wifi_set_channel(1, …)` statt Hopping, im Promiscuous-Callback `if (pkt->payload[0] & 0x80) return;`, `pBLEScan->setWindow(20)`).

---

## 🚀 Schnellstart & Installation

### Option A: WebSerial Flasher im Browser (Empfohlen)

Für das Flashen wird **kein Tooling** auf dem Ziel-PC benötigt!

1. Öffne die Seite [`web/flash.html`](web/flash.html) in einem WebSerial-fähigen Browser (Chrome, Edge, Brave oder Opera).
2. Schließe die drei **ESP32-C3 Super Minis** nacheinander per USB-Kabel an und klicke auf der Webseite jeweils auf **"Flashen 🔌"** für Station 1, Station 2 und Station 3.
3. Schließe danach deinen Peilsender an – **TENSTAR T-Display** *oder* **CYD 2.8"** – und klicke beim passenden Eintrag auf **"Flashen 🎯"**.

---

## ⚖️ Rechtliche Hinweise & Datenschutz (DSGVO / GDPR)

Da Funkwellen im 2.4 GHz Band (WLAN & Bluetooth) auch von Geräten unbeteiligter Dritter ausgesendet werden, wurde dieses System streng nach den Grundsätzen von **Privacy-by-Design** entwickelt:

1. **Kryptografische Anonymisierung (SHA-256)**:
   Jede erfasste MAC-Adresse wird **unmittelbar beim Empfang im Arbeitsspeicher des ESP32 mit SHA-256 gehasht**. Es verlassen zu keinem Zeitpunkt unverschlüsselte Quell-MAC-Adressen, Namen oder Inhaltspakete die Mikrocontroller.
2. **Lokale Funkverarbeitung**:
   Die Signalstärken werden ausschließlich lokal über das geschlossene ESP-NOW Protokoll zwischen den eigenen 4 Geräten ausgetauscht. Es findet **keinerlei Internetverbindung oder Datenweitergabe** statt.

---

## 🤖 KI-Assistenz & Antigravity Hinweis

Dieses Projekt wurde in enger Pair-Programming-Zusammenarbeit mit **Antigravity** (der agentischen KI-Entwicklungsumgebung von Google DeepMind) entwickelt. Von der Verfeinerung des C++ Promiscuous-Sniffings über die flimmerfreie ST7789 Render-Engine bis hin zur Dual-Trilateration und dem WebSerial Flasher wurde der gesamte Code von Mensch und KI gemeinsam konzipiert, programmiert und getestet.

---

## 📜 Lizenz & Danksagung

### Danksagung an die ursprünglichen Entwickler
Dieses Projekt baut auf den Grundlagen und Ideen des Projekts [`esp32-wifi-sniffer`](https://github.com/patrickhaahr/esp32-wifi-sniffer) auf. 

Ein herzlicher Dank geht an die ursprünglichen Autoren und Ersteller:
* **Patrick Haahr** ([@patrickhaahr](https://github.com/patrickhaahr))
* **Bananainsane**
* **FrostyCave**

### GitHub Repository & Branches
* Main Repository: [`https://github.com/TJRSchmidbauer/esp32-wifi-BLE-locator`](https://github.com/TJRSchmidbauer/esp32-wifi-BLE-locator)
* Branch `BLE-WLAN`: einziger Branch – enthält T-Display 1.14" 90° Radar Edition und CYD 2.8" Touch 2D-Raumplan Edition

### Lizenz
Dieses Projekt steht unter den Bedingungen der **MIT-Lizenz** (übernommen aus dem Original-Repository).
