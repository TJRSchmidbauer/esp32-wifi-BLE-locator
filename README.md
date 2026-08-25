# ESP32 Dual WiFi & BLE 90° Radar & Relativ-Ortungssystem (T-Display Edition)

Ein autarkes, datenschutzkonformes Ortungssystem auf Basis von C++ (Arduino ESP32 Core) für den ESP32. Das System erfasst zeitgleich **WLAN Probe Requests über das gesamte 13-Kanal-Spektrum** und **Bluetooth Low Energy (BLE) Advertisements** (Smartphones, Smartwatches, In-Ear-Headsets, AirTags oder BLE Beacons) und ermöglicht die **2D-Relativ-Ortung im Raum** (Messung der Entfernung direkt zu deinen Händen!) sowie die **Richtungsführung per 90° Sektor-Radar** auf einem **1.14" LilyGO / TENSTAR T-Display** – ganz **ohne PC, ohne Server und ohne WLAN-Router!**

---

### 💡 Verfügbare Hardware-Editionen im Repository

* 🎯 **`BLE-WLAN` Branch (Diese Branch)**: Entwickelt für das **1.14" LilyGO / TENSTAR T-Display ESP32** mit 90° Sektor-Radar und dynamischer Blickrichtungs-Führung.
* 📱 **[`master` Branch](https://github.com/TJRSchmidbauer/esp32-wifi-BLE-locator)**: Entwickelt für das **2.8" CYD Touch-Display (ESP32-2432S028)** mit 320x240 LCD, starrem 2D-Raumplan, interaktiver Touch-Geräteauswahl und Touch-Kalibrierung.

---

> 🤖 **Entwicklungs-Hinweis**: Dieses Repository und das gesamte C++ / WebSerial-System werden mit Unterstützung von **Google Antigravity** (einer fortschrittlichen agentischen KI für Pair-Programming von Google DeepMind) entwickelt, optimiert und gewartet.

---

## 🛒 Benötigte Hardware & 3D-Druck Gehäuse

| Komponente | Rolle im System | Produkt-Link (AliExpress) | 🖨️ 3D-Druck Gehäuse (MakerWorld) |
| :--- | :--- | :--- | :--- |
| **TENSTAR / LilyGO T-Display ESP32** | Handheld Peilsender mit 1.14" IPS LCD Display | 🔗 [AliExpress T-Display kaufen](https://de.aliexpress.com/item/1005005970553639.html?gatewayAdapt=glo2deu) | 📦 [LilyGO T-Display 1.14" Case (MakerWorld)](https://makerworld.com/de/models/1921314-lilygo-t-display-1-14-case#profileId-2061606) |
| **3x ESP32-C3 Super Mini** | Ecken-Stationen (13-Kanal WiFi + BLE Sniffer) | 🔗 [AliExpress ESP32-C3 Super Mini kaufen](https://de.aliexpress.com/item/1005006599448997.html?spm=a2g0o.order_list.order_list_main.55.514f5c5fRjT0iT&gatewayAdapt=glo2deu) | 📦 [SuperMini Snap-Fit Case (MakerWorld)](https://makerworld.com/de/models/2851590-esp32-s3-supermini-case-snap-fit-options#profileId-3180623) |

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
* 📶 **Full-Spectrum Channel Hopping (Kanal 1–13)**: Erfasst alle WLAN-Kanäle 1 bis 13 und scannte BLE mit 97,5 % Duty-Cycle (für Bluetooth-Headsets, Smartwatches & Tags).
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

## 🚀 Schnellstart & Installation

### Option A: WebSerial Flasher im Browser (Empfohlen)

Für das Flashen wird **kein Tooling** auf dem Ziel-PC benötigt!

1. Öffne die Seite [`web/flash.html`](web/flash.html) in einem WebSerial-fähigen Browser (Chrome, Edge, Brave oder Opera).
2. Schließe die drei **ESP32-C3 Super Minis** nacheinander per USB-Kabel an und klicke auf der Webseite jeweils auf **"Flashen 🔌"** für Station 1, Station 2 und Station 3.
3. Schließe danach das **TENSTAR T-Display Board** an und klicke auf **"Flashen 🎯"** für den Peilsender.

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
* BLE-WLAN Branch: T-Display 1.14" 90° Radar Edition
* Master Branch: [`https://github.com/TJRSchmidbauer/esp32-wifi-BLE-locator`](https://github.com/TJRSchmidbauer/esp32-wifi-BLE-locator) (CYD 2.8" Touch Display 2D Floorplan Edition)

### Lizenz
Dieses Projekt steht unter den Bedingungen der **MIT-Lizenz** (übernommen aus dem Original-Repository).
