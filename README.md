# Auto Mate

Auto Mate is the working product repository for a smart-room controller. The current confirmed intent is coordinated control of lighting and fans, including brightness and color-temperature control, environmental sensing, Home Assistant integration, and a local/custom web interface.

## Current status

This repository contains the product foundation only. No GPIO assignments, controller model, electrical schematic, PCB, enclosure, or production safety decisions have been approved yet.

## Repository boundaries

- `SPECIFICATIONS.md` — source of truth.
- `firmware/esphome/` — future ESPHome product configuration and modules.
- `hardware/kicad/` — future KiCad projects and review artifacts.
- `tests/` — firmware, integration, UX, and hardware-review evidence.
- `docs/` — architecture, AI context, gates, traceability, and release evidence.
- `releases/` — approved release notes and artifacts.
- `backups/` — local working backups; not public release artifacts.

## Safety and review boundary

AI may prepare candidate firmware, schematics, netlists, PCB outlines, placement, and validation reports. A human must approve final GPIO mappings, electrical connections, component choices, load switching, placement, routing, safety behavior, and release.

## Security

Never commit Wi-Fi credentials, Home Assistant tokens, API keys, customer data, or private commercial information. Keep secrets outside Git and use documented configuration templates.
