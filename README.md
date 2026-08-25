# ESP32 Dual WiFi & BLE 2D Raum-Ortungssystem (CYD Touch Edition)

Ein autarkes, datenschutzkonformes Ortungssystem auf Basis von C++ (Arduino ESP32 Core) für den ESP32. Das System erfasst zeitgleich **WLAN Probe Requests über das gesamte 13-Kanal-Spektrum** und **Bluetooth Low Energy (BLE) Advertisements** (Smartphones, Smartwatches, In-Ear-Headsets, AirTags oder BLE Beacons) und ermöglicht die **starre 2D-Raum-Ortung** auf einem **2.8" CYD Touch-Display (ESP32-2432S028)** – ganz **ohne PC, ohne Server und ohne WLAN-Router!**

---

### 💡 Verfügbare Hardware-Editionen im Repository

* 📱 **`master` Branch (Diese Branch)**: Entwickelt für das **2.8" CYD Touch-Display (ESP32-2432S028)** mit 320x240 LCD, starrem 2D-Raumplan, interaktiver Touch-Geräteauswahl und Touch-Kalibrierung.
* 🎯 **[`BLE-WLAN` Branch](https://github.com/TJRSchmidbauer/esp32-wifi-BLE-locator/tree/BLE-WLAN)**: Entwickelt für das **1.14" LilyGO / TENSTAR T-Display ESP32** mit 90° Sektor-Radar und dynamischer Blickrichtungs-Führung.

---

> 🤖 **Entwicklungs-Hinweis**: Dieses Repository und das gesamte C++ / WebSerial-System werden mit Unterstützung von **Google Antigravity** (einer fortschrittlichen agentischen KI für Pair-Programming von Google DeepMind) entwickelt, optimiert und gewartet.

---

## 🛒 Benötigte Hardware & 3D-Druck Gehäuse

| Komponente | Rolle im System | Produkt-Link (AliExpress) | 🖨️ 3D-Druck Gehäuse (MakerWorld) |
| :--- | :--- | :--- | :--- |
| **ESP32 CYD (Cheap Yellow Display ESP32-2432S028)** | Handheld 2.8" ILI9341 Color LCD Display | 🔗 [AliExpress CYD Display kaufen](https://de.aliexpress.com/item/1005006599448997.html) | 📦 [ESP32 CYD 2.8" Snap-Fit Case (MakerWorld)](https://makerworld.com/de/models/3180623) |
| **3x ESP32-C3 Super Mini** | Ecken-Stationen (13-Kanal WiFi + BLE Sniffer) | 🔗 [AliExpress ESP32-C3 Super Mini kaufen](https://de.aliexpress.com/item/1005006599448997.html) | 📦 [SuperMini Snap-Fit Case (MakerWorld)](https://makerworld.com/de/models/2851590-esp32-s3-supermini-case-snap-fit-options#profileId-3180623) |

---

## 📸 System-Übersicht & 2D-Raum-Aufbau

```
                         Station 2 (Vorne / Mitte)
                                  [ 📡 ]
                                    ▲
                                   / \
                                  /   \
                                 /     \
                                /   📱  \  ◄── Gesuchtes Zielgerät
                               /   (•)   \     (wird 2D trianguliert)
                              /           \
                             /   ┌─────┐   \
                            /    │ CYD │    \  ◄── 2.8" CYD Touch-Display
                           /     └─────┘     \
                          /                   \
                         /                     \
                        ▼                       ▼
                     [ 📡 ]                   [ 📡 ]
                Station 1 (Links)        Station 3 (Rechts)
```

Das System besteht aus genau **4 ESP32-Platinen**:
1. **3x ESP32-C3 Super Mini (Feste Ortungsstationen in den Ecken)**:
   - Station 1 (`station1`), Station 2 (`station2`) und Station 3 (`station3`).
   - Erfassen simultan WLAN-Signale über alle 13 Kanäle und Bluetooth Advertisements mit 97.5% Duty-Cycle.
2. **1x ESP32 CYD ESP32-2432S028 (Handheld 2D Floorplan Tracker)**:
   - Sendet periodische ESP-NOW Pings an die 3 Stationen, trianguliert zeitgleich die Position des Handhelds $(X_U, Y_U)$ und aller Zielgeräte $(X_T, Y_T)$ starr im 2D-Raumplan.

---

## 📏 Empfohlene Raumgrößen & Ortungsgenauigkeit

| Raumgröße | Abstand der Stationen | Ortungsverhalten & Präzision | Bewertung |
| :--- | :--- | :--- | :--- |
| **Unter 2m × 2m** *(< 4 m²)* | $< 1,5\text{ m}$ | ⚠️ **Zu klein**: Signale überlagern sich. Das Display zeigt das Ziel meist nur starr in der Mitte an. | ❌ Nicht empfohlen |
| **3m × 3m** *(9 m²)* | $\approx 2,5\text{ m}$ | 🟢 **Geringste sinnvolle Mindestgröße**: Triangulation funktioniert gut. Genauigkeit im Raum ca. $\pm 0,5\text{m}$ bis $\pm 0,8\text{m}$. | ✅ **Mindestgröße** |
| **4m × 5m** *(20 m²)* | $\approx 3,5\text{ m}$ | 🌟 **Optimaler Bereich (Standard-Zimmer)**: Perfekte Signaltrennung. Sehr gute 2D-Lokalisierung! | ⭐ **Ideal** |
| **6m × 8m** *(48 m²)* | $\approx 5,5\text{ m}$ | 🟢 **Sehr gut (Großer Raum / Büro)**: Volle Funktion bis ca. 10m Reichweite pro Station. | ✅ Sehr gut |

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

---

## ✨ Hauptmerkmale

* 🖐️ **2.8" CYD Touch-Display Unterstützung**: Optimiert für das **ESP32-2432S028 CYD (Cheap Yellow Display)** mit 320x240 Farbbildschirm und XPT2046 SPI Touch Controller.
* 🗺️ **Starre 2D-Raumplan-Darstellung**: Stellt den Raum mit Wänden, Stationen-Icons und Blips (🟢 WLAN, 🟣 BLE, 🟡 Ziel) visuell dar.
* 🎯 **Interaktive Touch-Auswahl**: Tippen direkt auf ein Blip auf dem Touchscreen wählt das Gerät aus. Die Touch-Buttons `[NEXT]` und `[CALIB]` steuern die Zielwahl und Raumkalibrierung.
* 📶 **Full-Spectrum Channel Hopping (Kanal 1–13)**: Erfasst alle WLAN-Kanäle 1 bis 13 und scannte BLE mit 97,5 % Duty-Cycle.
* 📏 **"Heiß / Kalt"-Entfernung zu deinen Händen**: Misst die direkte Relativ-Distanz zwischen deinen Händen und dem Zielgerät.
* 🛡️ **Paket-Wiederholungs-Filter (Hit-Count >= 3)**: Erfordert mindestens 3 empfangene Pakete und $\ge 2$ Stationen, um Phantom-Signale zu 100% auszufiltern.
* 📺 **Flimmerfreie Delta-Render-Engine**: Vermeidet zeilenweises SPI-Nachladen – das Bild steht absolut starr und gestochen scharf.
* 💡 **RGB-LED Stummschaltung**: Schaltet die störende Rückseiten-LED des CYD-Boards komplett aus.
* 🔒 **Privacy-First (DSGVO-Konform)**: Eindeutige MAC-Adressen werden noch **direkt auf dem ESP32-Chip per SHA-256 gehasht**. Es werden keine Klardaten oder Paketinhalte übertragen oder gespeichert.

---

## 🚀 Schnellstart & Installation

### Option A: WebSerial Flasher im Browser (Empfohlen)

Für das Flashen wird **kein Tooling** auf dem Ziel-PC benötigt!

1. Öffne die Seite [`web/flash.html`](web/flash.html) in einem WebSerial-fähigen Browser (Chrome, Edge, Brave oder Opera).
2. Schließe die drei **ESP32-C3 Super Minis** nacheinander per USB-Kabel an und klicke auf der Webseite jeweils auf **"Flashen 🔌"** für Station 1, Station 2 und Station 3.
3. Schließe danach das **ESP32 CYD Board** an und klicke auf **"Flashen 🎯"** für den Peilsender.

### Option B: Terminal-Skript (`flash_board.sh`)

Stecke nacheinander deine Boards per USB an und führe im Hauptverzeichnis aus:

```bash
# Station 1 flashen
./flash_board.sh station1

# Station 2 flashen
./flash_board.sh station2

# Station 3 flashen
./flash_board.sh station3

# CYD 2D Ortung Tracker flashen
./flash_board.sh tracker
```

---

## ⚖️ Rechtliche Hinweise & Datenschutz (DSGVO / GDPR)

Da Funkwellen im 2.4 GHz Band (WLAN & Bluetooth) auch von Geräten unbeteiligter Dritter ausgesendet werden, wurde dieses System streng nach den Grundsätzen von **Privacy-by-Design** entwickelt:

1. **Kryptografische Anonymisierung (SHA-256)**:
   Jede erfasste MAC-Adresse wird **unmittelbar beim Empfang im Arbeitsspeicher des ESP32 mit SHA-256 gehasht**. Es verlassen zu keinem Zeitpunkt unverschlüsselte Quell-MAC-Adressen, Namen oder Inhaltspakete die Mikrocontroller.
2. **Lokale Funkverarbeitung**:
   Die Signalstärken werden ausschließlich lokal über das geschlossene ESP-NOW Protokoll zwischen den eigenen 4 Geräten ausgetauscht. Es findet **keinerlei Internetverbindung oder Datenweitergabe** statt.

---

## 🤖 KI-Assistenz & Antigravity Hinweis

Dieses Projekt wurde in enger Pair-Programming-Zusammenarbeit mit **Antigravity** (der agentischen KI-Entwicklungsumgebung von Google DeepMind) entwickelt. Von der Verfeinerung des C++ Promiscuous-Sniffings über die flimmerfreie ILI9341 Render-Engine bis hin zur Dual-Trilateration und dem WebSerial Flasher wurde der gesamte Code von Mensch und KI gemeinsam konzipiert, programmiert und getestet.

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
* Master Branch: CYD 2.8" Touch Display 2D Floorplan Edition
* BLE-WLAN Branch: [`https://github.com/TJRSchmidbauer/esp32-wifi-BLE-locator/tree/BLE-WLAN`](https://github.com/TJRSchmidbauer/esp32-wifi-BLE-locator/tree/BLE-WLAN) (LilyGO T-Display 1.14" 90° Radar Edition)

### Lizenz
Dieses Projekt steht unter den Bedingungen der **MIT-Lizenz** (übernommen aus dem Original-Repository).
