# Home Telemetry

A personal **home telemetry layer**: distributed wireless sensors around the apartment, collecting environmental data to a small always-on local server that stores it, visualizes it, and eventually triggers physical feedback and automation.

**Status:** Initial hardware acquisition / architecture planning.

> The server isn't the project. The project is building sensing + physical feedback that removes small daily annoyances — not another dashboard nobody checks.

---

## Why this exists

It started as "buy a small always-on server" (Jellyfin, Docker playground, self-hosted stuff). After thinking it through, the real motivation shifted: the interesting part isn't the box, it's the **telemetry layer** on top of it — sensors, local data collection, physical status indicators, and automation triggered by real-world events.

The guiding test for any feature:

> *"I was annoyed by this repeatedly, so I automated it."*

Concrete annoyances to target:

- Washing-machine-finished notification
- Room temperature / humidity monitoring
- Small weather station
- Battery monitoring on sensor nodes
- Physical LED status indicators
- Apartment environmental heatmaps

It's also a deliberate long-term engineering playground spanning: embedded/ESP32 firmware, Go/Rust backend services, networking, electronics, local infrastructure, and physical automation.

---

## Architecture

```
ESP32 sensor nodes
      │  WiFi
      ▼
HP T630 home server
      ├── MQTT (Mosquitto)
      ├── Data storage (SQLite)
      ├── API
      ├── Web UI
      ├── Notifications
      └── LED status board
```

- **[docs/hardware.md](docs/hardware.md)** — server, sensor nodes, battery system, inventory, shopping list
- **[docs/roadmap.md](docs/roadmap.md)** — milestones, first prototype, data model, future project ideas

---

## Repo layout

```
home-telemetry/
├── README.md        # this file — vision + architecture
├── docs/            # hardware, roadmap, data model
├── firmware/        # ESP32-C3 sensor node firmware (TBD)
└── server/          # MQTT ingest + storage + API (Go/Rust, TBD)
```

---

## First milestone — one complete loop

Do **not** start with 8 sensors, batteries, waterproofing, or dashboards. Build the whole vertical slice once, end to end:

```
ESP32-C3 + BME280 (USB power)
      │  WiFi
      ▼
MQTT → HP T630 → store reading
```

Immediate next steps:

1. Set up HP T630 server — provision NixOS declaratively via a flake
2. Run MQTT broker (Mosquitto)
3. Connect one ESP32-C3
4. Read BME280
5. Send a JSON payload
6. Store the data
7. Build a minimal local API
8. Add visualization **only after data exists**

---

## Development philosophy

**Avoid:**

- Buying hardware without a project for it
- Building dashboards nobody checks
- Unnecessary self-hosted replacements
- Smart-home complexity for its own sake

**Prefer:**

- Solving small, repeated annoyances
- Physical feedback over screens
- Tools used daily
- Small incremental improvements

Build the first complete loop before anything else.
