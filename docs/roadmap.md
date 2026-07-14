# Roadmap

## First prototype — single working sensor

The first milestone is **one complete loop**, not a fleet.

**Hardware:** ESP32-C3 + BME280 + USB power

**Software path:**

```
ESP32 → WiFi → MQTT → HP T630 → store reading
```

**Expected payload:**

```json
{
  "device": "bedroom",
  "temperature": 22.5,
  "humidity": 52,
  "pressure": 1012
}
```

---

## Data model

The system should be **event-based** — avoid hardcoding specific projects into the schema.

```json
{
  "device": "bedroom",
  "event": "environment",
  "temperature": 22.5,
  "humidity": 52,
  "timestamp": 1750000000
}
```

Other events reuse the same envelope:

```json
{ "device": "laundry", "event": "finished" }
```

```json
{ "device": "window", "event": "opened" }
```

---

## LED status system (future)

A physical status board — glanceable state without opening a screen.

```
+----------------+
|  SENSOR STATUS |
+----------------+
  Bedroom  🟢
  Outside  🟢
  Window   🔴
  Battery  🟡
  Server   🟢
```

| State        | Meaning              |
|--------------|----------------------|
| Green        | Healthy              |
| Green blink  | Sending data         |
| Yellow       | Warning / low battery|
| Red          | Offline / error      |

Possible implementation: `Server → WiFi/API → Arduino Nano → LED board` (driving the SK6812 strip).

---

## Future project ideas

### Weather station

Sensors: temperature, humidity, pressure, rain, wind.
Output: local website, historical graphs, weather trends.

### Apartment heatmap

Multiple sensors (bedroom, living room, window, outside, desk) → temperature map, humidity map, airflow observations.

### Laundry notification

ESP32 + sensor detecting vibration / power usage / state change → "laundry finished" via Discord, Telegram, or a local notification.

### Personal event timeline

Instead of task management, track **environment events** — build a memory of the space:

```
20:31  Bedroom: 23°C
20:25  Window opened
20:10  Sensor battery updated
19:50  Outside temperature dropped
```

---

## Immediate next steps

1. Set up HP T630 server
2. Install NixOS (flake-based, declarative)
3. Run MQTT broker
4. Connect one ESP32-C3
5. Read BME280
6. Send JSON payload
7. Store data
8. Build minimal local API
9. Add visualization only after data exists
