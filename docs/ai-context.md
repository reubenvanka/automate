# AutoMate AI Context

## Product

- Name: **AutoMate**
- Slug: `automate`
- Deployment: Home Office used by a video-editing team
- Specification: `SPECIFICATIONS.md`
- Architecture: `docs/architecture.md`
- Plan: `PROJECT_PLAN.md`
- Release evidence: `docs/release-evidence.md`

## Confirmed intent

AutoMate coordinates Home Office lighting, fan, desk-power, stabilizer, appliance, environmental, door, current, and voltage systems. The ESP32 must execute relay/PWM dependency automations locally even when Home Assistant, the web client, or the future custom ESP remote is unavailable.

The confirmed inventory is an ESP32 38-pin C-type USB development board marked ETD11489 with 4 MB flash, nine relay functions implemented as an eight-channel 5 V active-LOW relay module plus a separate 30 A AC relay, 24 PWM outputs, three CCT fixtures, AHT20, BMP280, BH1750, MC-38, ZMCT103C, and ZMPT101B. The three CCT fixtures are Ceiling, Desk 1 Light Bar, and Desk 2 Light Bar, each with warm and cold channels. The approved transport priority assigns ten ESP32 hardware PWM/LEDC channels (six CCT plus four single-channel: Photo Spot, Wall Bounce, Washroom, Terrace) and the remaining fourteen single-channel outputs to a single PCA9685 with two named backup channels (CH14/CH15); software/bit-banged PWM is prohibited. Exhaust Fans is one shared PCA9685 control output for two roof fans connected in parallel. One 74HC595-family register drives the eight-channel module inputs, while the 30 A relay uses a separate ESP32 GPIO; the register drives module inputs, not relay coils. The ESP32 controls the register through `SER`, `SRCLK`, and `RCLK` only. `OE/G` is tied low and `SRCLR/MR` is tied high; these fixed biases consume no GPIOs but do not by themselves prove passive default-off behavior. A standard 5 V-powered 74HC595 does not guarantee recognition of a 3.3 V ESP32 HIGH, so a 74HCT595 or verified level-shifting arrangement is preferred. Load, energy, and sensor-health monitoring is derived in software from the reviewed current/voltage measurements; no dedicated metering IC is planned. Home Assistant-managed scenes are persisted in bounded storage through native-API services, and ESP-NOW is deferred rather than approved.

## Current blockers

- Exact ESP32 module/revision, board-level schematic, and physical pinout for the ETD11489 board: TODO; the board description and 4 MB flash are confirmed.
- Exact GPIO/peripheral mapping: TODO; verify against the exact module and account for LEDC capacity, flash-connected pins, input-only pins, ADC2/Wi‑Fi restrictions, strapping pins, and UART flashing/debug pins.
- Exact relay-module channel mapping, load schedule, ratings, wiring, and active-LOW behavior: TODO; the eight-module-channel plus separate 30 A relay architecture is confirmed.
- 74HC595-family/74HCT595 relay-input current, 3.3 V-to-5 V logic compatibility, fixed `OE/G`-low/`SRCLR/MR`-high biases, external default-off network, reset/brownout/register-corruption behavior, grounding, and fault behavior: TODO.
- D4814 datasheet, input logic, PWM compatibility, current/thermal limits: TODO.
- Exact PCA9685 address, I2C topology, output-drive behavior, OE strategy, and power sequencing for the single 16-channel device (CH0–CH13 assigned, CH14/CH15 backup): TODO.
- Power budget, regulators, inrush, protection, grounding, isolation, and thermal design: TODO.
- ZMCT103C/ZMPT101B ADC1 interface circuit, isolation/conditioning, calibration, and qualified electrical review: TODO; ADC1 is the selected measurement direction.
- Camera-flicker target frequencies and actual camera/load test matrix: TODO.
- Maintenance override persistence, timeout, and release semantics: TODO.
- Home Assistant entity/native-API scene-service contract, authenticated local web contract, bounded scene persistence/corruption handling, and future remote protocol: TODO. ESP-NOW is deferred; transport selection is TBD.
- UX details for indicators, display, locks, undo, animations, and restarts: TODO.

## Toolchain

- ESPHome: product-approved version and packages from `platform/iot-platform`.
- KiCad: installed KiCad 10.0.6 and `/opt/homebrew/bin/kicad-cli`.
- MCP: modern-Python server environment validated for startup, 45 registered tools, project creation, PCB outline setup, ERC, and DRC; PCB API limitations remain visible.

## Review boundary

AI-generated firmware, schematics, connections, placement, routing, and validation reports are candidates until reviewed and tested. Human approval is required for GPIO, electrical, safety, hardware, and release decisions. Never commit secrets, credentials, customer data, or private commercial information.
