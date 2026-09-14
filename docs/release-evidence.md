# AutoMate Release Evidence

## Current foundation release

- Product: **AutoMate**
- Date: 2026-09-11
- Scope: Repository foundation, Home Office requirements/specification, architecture, AI context, rules, traceability, gates, secret-safe ignore policy, ETD11489 ESP32 board description, one-register plus direct-GPIO relay topology, fixed `OE/G`-low/`SRCLR/MR`-high relay-control biases, confirmed Desk 1/Desk 2 light-bar naming, shared parallel exhaust-fan control, 74HC595/74HCT595 logic-level finding, ten ESP32-direct LEDC channels (six CCT plus four single-channel), fourteen PCA9685 single-channel outputs plus two backup channels, prohibition on software/bit-banged PWM, software-derived load/energy/sensor-health monitoring, persistent Home Assistant-managed scenes through native-API services, safe desired-state restoration, ESP-NOW deferral, and ADC1-based measurement direction.
- Product implementation: None.
- Hardware approval: Not applicable yet; hardware decisions remain TODO.
- MCP/KiCad foundation validation: Project creation, 50 × 40 mm PCB outline setup, ERC, and DRC passed in a temporary validation project.
- GitHub publishing: repository renamed to `https://github.com/reubenvanka/automate`; local `origin` now points to the renamed repository and retains the existing `main` history.
- Human release approval: Pending; this is a foundation commit, not a product release.

## Evidence checklist

- [x] Repository inspection completed.
- [x] No secrets or credentials committed.
- [x] Home Office requirements, automation semantics, architecture, and traceability reviewed.
- [x] MCP/KiCad foundation workflow validated.
- [x] Hardware TODOs remain visible and block hardware-dependent work.
- [x] ETD11489 ESP32 board description, 4 MB flash, one-register plus direct-GPIO relay topology, fixed `OE/G`-low/`SRCLR/MR`-high biases, confirmed Desk 1/Desk 2 light-bar naming, shared parallel exhaust-fan control, 74HC595/74HCT595 logic-level finding, ten ESP32-direct LEDC channels (six CCT plus four single-channel), fourteen PCA9685 single-channel outputs plus two backup channels, software/bit-banged PWM prohibition, software-derived monitoring, persistent native-API scenes, safe restoration, ESP-NOW deferral, and ADC1 measurement direction recorded in the source-of-truth documents.
- [x] Pin/peripheral budget worksheet created without assigning GPIO numbers.
- [ ] Human approval recorded before any product release.

## Firmware validation & build (2026-09-14)

**Environment:** ESPHome CLI `2026.4.5` installed locally (via pip); ESP-IDF framework 5.5.4.

| Check | Command | Result |
| --- | --- | --- |
| Config validation | `esphome config configuration.yaml` | **Passed** (exit 0; only 3 accepted strapping-pin warnings GPIO5/12/15) |
| Full build | `esphome compile configuration.yaml` | **Passed** — `Successfully compiled program.` |

**Artifacts (git-ignored):**
`firmware.bin`, `firmware.ota.bin`, `firmware.factory.bin` under `.esphome/build/automate-controller/.pioenvs/automate-controller/` (copies in `firmware/esphome/build/`).

**Coverage in build:**
- 10 ESP32 LEDC lights (6 CCT via `cwww` + Photo Spot / Wall Bounce / Washroom / Terrace) + 14 PCA9685 channels + `BACKUP_01/02`.
- 8 relays via native `sn74hc595` (GPIO20/23/15) + direct 30A relay (GPIO32).
- Sensors: AHT20 (as `aht10`/variant), BMP280 (`bmp280_i2c`), BH1750, door (GPIO14), ZMCT103C/ZMPT101B (ADC1), VS1838B IR (`nec`).
- `restore_mode: RESTORE_DEFAULT_OFF` on all 23 lights and 9 relays (local NVS restore, no HA required).
- `secrets.yaml` populated (git-ignored) with live Wi-Fi/static-IP/API/web credentials.

**ESPHome-hardware-constraint GPIO changes from the original budget:**
- 74HC595 latch/RCLK → **GPIO15** (GPIO24 is not exposed in the ESP32 IO mux).
- Door sensor → **GPIO14** (GPIO34–39 do not support `INPUT_PULLUP`).
- **74HC595 serial data (SER) → GPIO0** (GPIO20 is NOT broken out on the ESP32-WROOM DevKit header; GPIO0 was freed after removing the PCA9685 OE; GPIO0 is the BOOT/strapping pin, no external pull).

**Known limitations carried in this build:**
- `state_machine.yaml` **not included**: it relied on `custom_component:`, which ESPHome removed; scene persistence/restore API services are stubbed logs and need porting to an external component.
- Hardware electrical review (relay default-off proof, strapping-pin intent, header) still **pending human approval**.

## Future release index

| Area | Command/artifact | Result | Reviewer |
| --- | --- | --- | --- |
| Firmware build | `esphome compile configuration.yaml` | Passed (config exit 0; build succeeded) | Human review pending |
| Firmware tests | TODO | TODO | TODO |
| Home Assistant integration and persistent scene services | TODO | TODO | TODO |
| Local web integration | TODO | TODO | TODO |
| Safe restoration, recovery, and software-derived diagnostics | TODO | TODO | TODO |
| Future ESP remote protocol (ESP-NOW deferred) | TODO | TODO | TODO |
| UX/failure-mode review | TODO | TODO | TODO |
| ERC | Temporary MCP smoke project | Passed | Human review pending |
| DRC | Temporary MCP smoke project | Passed | Human review pending |
| Secret/history scan | Repository inspection | Passed for foundation | Human review pending |

## Validation boundary

The validated workflow covers the MCP server's text/parser and `kicad-cli` path. KiCad API-backed PCB editing remains blocked by the Python 3.9/3.10+ `pcbnew` compatibility boundary. No final AutoMate GPIO numbers, PCA9685 addresses, or electrical connections have been approved; the fixed `OE/G`-low/`SRCLR/MR`-high biases are documented, but their electrical default-off behavior and all hardware, scene, recovery, and monitoring tests remain pending.
