# Auto Mate Architecture

**Source:** `SPECIFICATIONS.md` — update the specification before changing architecture.

## Product boundary

Auto Mate coordinates room lighting and fan/environmental control. It may expose approved state and commands through Home Assistant and a local/custom web interface.

## Firmware architecture

- Product assembly/configuration: TODO.
- Shared platform packages: TODO.
- Control-state model and validation: TODO.
- Home Assistant integration contract: TODO.
- Local web API/UI contract: TODO.
- Restart, reconnect, recovery, and invalid-command behavior: TODO.

## Hardware architecture

- Controller/MCU: TODO.
- GPIO/peripheral map: TODO.
- Power and load topology: TODO.
- Lighting/fan switching and fail-safe state: TODO.
- Sensors and calibration: TODO.
- Indicators/display/buttons/enclosure: TODO.

## Security architecture

- Secret storage: external/ignored local configuration; TODO implementation.
- Home Assistant authentication: TODO.
- Local web authorization and network exposure: TODO.
- Audit/logging and personal-data handling: TODO.

## Validation strategy

- Firmware build and unit/integration tests.
- Home Assistant/local web integration tests.
- UX and failure-mode review.
- KiCad netlist, ERC, DRC, and human hardware review.
- Release evidence and rollback/recovery review.
