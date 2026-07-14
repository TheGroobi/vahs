# V.A.H.S. — Voluntarily Ass Home Server

A small always-on home server for the apartment, configured declaratively with
NixOS. Runs on an HP T630 thin client; ESP32 sensors report to it over WiFi.

## Layout

- `server/` — NixOS config (flake) + Makefile
- `firmware/` — ESP32-C3 sensor firmware

## Build & run

The whole server is one flake. From `server/`:

```sh
make run      # build + boot the VM
make fresh    # same, but wipes the VM disk first
```

Log in as `groobi` / `test` (VM-only password). The real T630 gets provisioned
later with nixos-anywhere.

## Roadmap

- [x] NixOS flake boots as a local VM
- [ ] disko disk layout + deploy to the real T630 (nixos-anywhere)
- [ ] MQTT broker (Mosquitto)
- [ ] first ESP32-C3 + BME280 → JSON payload over WiFi
- [ ] store readings (SQLite)
- [ ] minimal local API
- [ ] visualization

Detailed notes, hardware BOM, and ideas live in `~/notes/projects/vahs.md`.
