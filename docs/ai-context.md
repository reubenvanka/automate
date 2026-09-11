# Auto Mate AI Context

## Product

- Name: **Auto Mate**
- Slug: `auto-mate`
- Specification: `SPECIFICATIONS.md`
- Architecture: `docs/architecture.md`
- Release evidence: `docs/release-evidence.md`

## Confirmed intent

Auto Mate is a smart-room controller for coordinated lighting and fan/environmental control. The current scope includes brightness and color-temperature control where supported, environmental sensing, Home Assistant integration, and a local/custom web interface.

## Current blockers

- Controller/MCU and hardware revision: TODO.
- GPIO/peripheral mapping: TODO.
- Power, load switching, ratings, protection, and fail-safe behavior: TODO.
- Sensor models and calibration: TODO.
- UX details for LEDs, display, locks, undo, animations, and restarts: TODO.
- Home Assistant and local web contracts: TODO.

## Toolchain

- ESPHome: product-approved version and packages from `platform/iot-platform`.
- KiCad: installed KiCad 10.0.6 and `/opt/homebrew/bin/kicad-cli`.
- MCP: modern-Python server environment validated for startup, 45 registered tools, project creation, PCB outline setup, ERC, and DRC; PCB API limitations remain visible.

## Review boundary

AI-generated firmware, schematics, connections, placement, routing, and validation reports are candidates until reviewed and tested. Human approval is required for hardware and release decisions.
