# Hardware

## Server — HP T630 Thin Client

**Purchased.** Always-on local infrastructure.

| Spec    | Value  |
|---------|--------|
| RAM     | 8 GB   |
| Storage | 128 GB |

Why the T630 over a Raspberry Pi:

- Cheaper than a Pi
- x86 → full Linux compatibility
- Enough performance for MQTT, databases, APIs, dashboards, automation services

### Planned server software

**Initial (keep it minimal):**

- NixOS — declarative, flake-based provisioning (chosen to try out Nix)
- Docker
- Mosquitto MQTT
- Go / Rust services
- SQLite
- Web API

**Possible later:** InfluxDB · Grafana · PostgreSQL · Home Assistant

> Avoid unnecessary infrastructure early. The goal is to build useful systems, not maintain a homelab.

---

## Sensor nodes

### Controller — Seeed XIAO ESP32-C3 (×2)

Why:

- Small, cheap
- WiFi + BLE
- Low power, good deep-sleep support
- Enough GPIO

Add more nodes only after the first working prototype.

### Environmental sensor — BME280

Measures **temperature · humidity · pressure** over **I2C**.

Initial use: indoor temp/humidity monitoring. Candidate locations: bedroom, desk, living room, outside.

---

## Battery system

Prototype on **USB power first** — do not optimize battery life immediately. Deep sleep, battery-voltage measurement, and low-battery notifications come later.

### Cell — Akyga Li-Pol

| Spec       | Value          |
|------------|----------------|
| Capacity   | 470 mAh        |
| Voltage    | 3.7 V (1S)     |
| Size       | 30×30×6 mm     |
| Protection | PCM (protected)|

### Charger — TP4056 HW-373 (USB-C)

1S Li-Ion charger with protection.

### Connectors — justPi JST PH 2.00 kit

Standardize removable wiring for batteries, sensors, and modules.

---

## Electronics inventory (already owned)

**Boards**

- Arduino Nano ×2
- ESP32-S3 development board

**Components**

- Breadboards
- Dupont wire roll
- Resistors
- Perfboards
- Transistor assortment
- IRLZ44N MOSFETs

**Lighting**

- SK6812 RGBW addressable LED strip — 5 V, 144 LEDs/m, IP65 (purchased; earmarked for the status-light system)

---

## Shopping decision

**Keep (initial hardware set):**

- 2× ESP32-C3 XIAO
- 1× BME280
- 1× 470 mAh LiPo
- 1× TP4056
- JST PH kit

**Do not buy yet:**

- More sensors
- More batteries
- Raspberry Pi
- Extra infrastructure

> Build the first complete loop first.
