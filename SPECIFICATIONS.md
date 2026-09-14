# AutoMate Specification

**Status:** Draft Home Office requirements and architecture specification

**Source of truth:** This file. Implementation and documentation must not introduce behavior that conflicts with it.

## Product identity

- Product name: **AutoMate**
- Repository slug: `automate`
- Product category: Smart-room controller
- Deployment: Home Office used by a video-editing team
- Target controller: **ESP32 38-pin C-type USB development board, marking ETD11489, with 4 MB flash** (user-provided); the underlying ESP32 module/revision, exact silicon pinout, and board-level schematic remain unverified.
- Current phase: Requirements, architecture, and hardware planning

## Confirmed product intent

AutoMate shall coordinate the Home Office lighting, fan, desk-power, stabilizer, and monitored appliance outputs. The controller shall keep the required relay and PWM automations running locally on the ESP32, even when Home Assistant, the local web client, or the future custom ESP remote is unavailable.

The confirmed scope is:

1. Control nine named relay functions.
2. Control 24 named PWM outputs, including three two-channel CCT light fixtures.
3. Provide brightness control for every dimmable output.
4. Provide color-temperature control for the ceiling light and two desk light bars.
5. Read environmental, light, door, current, and voltage sensors.
6. Integrate with Home Assistant through the ESPHome native API.
7. Provide an authenticated local web interface for approved control and status functions.
8. Reserve a documented interface for a future custom ESP remote.
9. Preserve safe, deterministic behavior during boot, restart, brownout, reconnect, invalid commands, sensor failure, and maintenance overrides.

These are product requirements, not approved electrical, PCB, or GPIO decisions.

## Home Office inventory

### Relay outputs

All relay control inputs are expected to use a 5 V trigger, active-LOW module, but the actual module behavior, input current, isolation, and boot state must be verified before wiring. The shift-register outputs drive only the relay-module input circuitry (the input LED/optocoupler/driver path); they never drive the relay coils directly. Therefore, relay-coil current is not a shift-register output-current requirement, but the exact module input current and logic thresholds remain requirements. The selected topology is one 8-bit 74HC595-family register for the eight-channel module and one separate ESP32 GPIO for the 30 A relay. A standard TI SN74HC595 powered at 5 V is not guaranteed to recognize a 3.3 V ESP32 HIGH: its specified input-HIGH minimum is 0.7 × VCC, which is 3.5 V at VCC = 5 V. A 74HCT595 is the preferred candidate for direct 3.3 V ESP32 control at a 5 V register supply because TI specifies VIH(min) = 2.0 V and VIL(max) = 0.8 V over 4.5 V to 5.5 V operation. The exact purchased part and relay-module interface still require datasheet review and bench validation.

A manual maintenance override/bypass is required so that a failed, powered-down, or overridden ESP32 controller can never prevent an operator from energizing a relay. This supports the confirmed maintenance-override behavior in item 9 of the product intent. The approved topology is a parallel GND override on each relay input: a 12-position DIP switch is interleaved into each relay channel such that switching it closed connects the relay-module input directly to GND (forcing the relay ON regardless of the ESP32 state), and a Schottky diode (1N5817-class) is placed in series between each 74HC595 output and the relay node to isolate the shift-register output from the override short. Neither the DIP switch nor the diode removes the active-LOW default-off pull (relay-module input pulled to VCC at reset); the override provides force-ON only and is not intended to force a relay OFF while the ESP32 is actively driving it ON.

| ID | Output | Required behavior | Candidate topology |
| --- | --- | --- | --- |
| AM-RLY-01 | Fan | Independently switchable fan relay. | One relay channel. |
| AM-RLY-02 | Ceiling lights driver | Follow the logical ceiling-light on/off state; turn off when both ceiling PWM channels are off. | Upstream relay for the ceiling CCT driver. |
| AM-RLY-03 | 12 V driver | Turn on when any non-ceiling PWM output is active; turn off when all such outputs are inactive. | Upstream relay for the 12 V PWM supply. |
| AM-RLY-04 | Voltage stabilizer | Stay on when Desk 1, Desk 2, or Desk 3 is on; turn off only when all three desk relays are off. | Upstream relay for the desk-power branch. |
| AM-RLY-05 | Desk 1 | Independently switchable desk relay, subject to stabilizer availability. | One relay channel. |
| AM-RLY-06 | Desk 2 | Independently switchable desk relay, subject to stabilizer availability. | One relay channel. |
| AM-RLY-07 | Desk 3 | Independently switchable desk relay, subject to stabilizer availability. | One relay channel. |
| AM-RLY-08 | 16 A relay | Independently switchable 16 A-rated output; load and voltage are TBD. | Separate or module channel as approved. |
| AM-RLY-09 | AC relay | Independently switchable 30 A single-channel AC output; load and protection are TBD. | Separate 30 A relay module. |

**Confirmed planning architecture:** the nine relay functions use eight channels on the 5 V active-LOW relay module plus one separate 30 A single-channel AC relay. One 8-bit 74HC595-family register drives only the eight-channel module inputs; the 30 A relay uses one separate ESP32 GPIO and does not use a second register. The exact purchased-module channel mapping, load ratings, wiring, and protection remain TODO. The register outputs drive the relay-module input circuitry, not the relay coils. The ESP32 controls the register through three signals only: serial data (`SER`), shift clock (`SRCLK`), and storage-register/latch clock (`RCLK`). `OE/G` is tied low and `SRCLR/MR` is tied high; these fixed biases consume no ESP32 GPIOs. Tying `OE/G` low enables the register outputs and tying `SRCLR/MR` high deasserts clear; neither bias by itself proves that the relay inputs are off. A verified relay-input pull/default-off network and reset/brownout/register-corruption tests are required to prove that all relay inputs remain inactive during ESP32 reset, brownout, firmware initialization, and register corruption. The ready relay modules (eight-channel and 30 A) provide their own input pull/default-off networks internally, so no external pull-ups are added; the D4814 trigger modules are fail-safe OFF; and the PCA9685 module straps OE LOW (outputs always enabled). Optocoupler isolation does not inherently require a common ground, but the exact relay-module input circuit and return-current path must be verified.

### PWM outputs

The inventory contains two separate light bars: Desk 1 Light Bar and Desk 2 Light Bar. Each has warm and cold channels; the six CCT PWM outputs are Ceiling Warm/Cold, Desk 1 Light Bar Warm/Cold, and Desk 2 Light Bar Warm/Cold.

The required logical outputs are:

- CCT warm/cold pairs: Ceiling, Desk 1 Light Bar, Desk 2 Light Bar.
- Single-channel dimmable outputs: Monitor 1 Backlight, Desk Light 1, Table Light, Shelf Light 1, Monitor 2 Backlight, Desk Light 2, Shelf Light 2, Wall Bounce Light, Photo Spot Light, Cupboard Bounce Light, Cupboard Spot Light, Door Spot Light, Monitor Light 3, Monitor Light 4, Exhaust Fans, Washroom Light, Terrace Light, Terrace Ambient Light.
- Total: 24 PWM outputs, comprising six CCT channels and eighteen single-channel outputs.

The approved transport priority is:

- Keep six CCT channels on ESP32 hardware PWM/LEDC (Ceiling Warm/Cold, Desk 1 Warm/Cold, Desk 2 Warm/Cold).
- Keep four single-channel outputs on ESP32 hardware PWM/LEDC: Photo Spot Light, Wall Bounce Light, Washroom Light, Terrace Light (total ten ESP32-direct PWM outputs).
- Route the remaining fourteen single-channel outputs through a single PCA9685. The two roof exhaust fans remain one shared PCA9685 control output because they are connected in parallel and independent fan speed control is not part of the confirmed topology. The two unused PCA9685 channels (CH14, CH15) are carried as named backup channels (BACKUP_01, BACKUP_02) and remain assigned so that future devices can be mapped without restructuring.
- Software or bit-banged PWM is prohibited; every dimmable output must use ESP32 hardware LEDC or PCA9685.

This gives ten ESP32-direct logical PWM outputs and fourteen PCA9685 logical outputs plus two PCA9685 backup channels, for a total of 24 PWM output functions across twenty-six assigned channels (24 active + 2 backup). One 16-channel PCA9685 provides exactly sixteen channels (CH0–CH13 assigned, CH14–CH15 backup). This is a transport-priority and channel-allocation decision; the exact ESP32 module on the ETD11489 board is unverified, so the ten direct intents must be checked against the exact module's LEDC capacity before GPIO allocation. The GPIO mapping for the ten ESP32 outputs is recorded in `docs/pin-budget.md`. The shared exhaust-fan signal's input-current, drive, wiring, and protection requirements remain TODO.

For the pin review, verify the exact module and board schematic. For a classic ESP32, GPIO6–GPIO11 and GPIO16–GPIO17 are normally connected to module flash/PSRAM and should not be reused; GPIO34–GPIO39 are input-only; ADC2 pins cannot be used while Wi‑Fi is active; and strapping pins plus UART flashing/debug pins require board-level review. The current/voltage analog inputs should therefore use an approved ADC1-based conditioned interface after module verification.

### Sensors

| ID | Sensor | Required measurement | Candidate interface | Status |
| --- | --- | --- | --- | --- |
| AM-SEN-01 | AHT20 | Temperature and humidity | I2C; default address to be verified | Candidate |
| AM-SEN-02 | BMP280 | Temperature, pressure, and optional altitude-derived data | I2C; address to be verified | Candidate |
| AM-SEN-03 | BH1750 | Ambient illuminance in lux | I2C; address to be verified | Candidate |
| AM-SEN-04 | MC-38 magnetic contact | Door/window open/closed state | GPIO input with configurable NO/NC polarity and debounce | Candidate |
| AM-SEN-05 | ZMCT103C interface | AC current and derived power/energy | Conditioned/isolated ADC1 analog input; no metering IC planned | ADC1 architecture selected; safety/calibration review required |
| AM-SEN-06 | ZMPT101B interface | AC voltage and derived power/energy | Conditioned/isolated ADC1 analog input; no metering IC planned | ADC1 architecture selected; safety/calibration review required |

The selected architecture uses the ESP32 ADC1 with ZMCT103C current and ZMPT101B voltage interfaces. The raw sensor modules are not a complete measurement system by themselves: burden, biasing, filtering, ADC range, isolation, calibration, and enclosure design require qualified electrical review. Energy, load, and sensor-health values are calculated in software from the existing current and voltage sensors; no dedicated metering IC is planned. A later metering-IC change requires a reviewed design change.

## Requirements

| ID | Requirement | Acceptance evidence |
| --- | --- | --- |
| AM-REQ-001 | Provide coordinated control of lighting and fan outputs. | Approved control-state model and integration/manual tests. |
| AM-REQ-002 | Support brightness control where the selected hardware and load support it. | Hardware capability documented and brightness tests pass. |
| AM-REQ-003 | Support color-temperature control where supported. | Hardware capability documented and color-temperature tests pass. |
| AM-REQ-004 | Read and expose environmental sensor data. | Approved sensor model, calibration approach, and data tests. |
| AM-REQ-005 | Integrate with Home Assistant using a documented, secret-safe ESPHome native API. | Native-API integration and service tests; no credentials in Git. |
| AM-REQ-006 | Provide an authenticated local/custom web interface for approved control and status functions. | UI/API review and local integration tests. |
| AM-REQ-007 | Define safe behavior for restart, reconnect, unavailable integrations, invalid commands, brownout, and state restoration. | Boot, failure-mode, and restoration tests plus human UX/safety review. |
| AM-REQ-008 | Keep all secrets, credentials, personal/customer data, and private commercial information outside Git. | Secret scan and repository-history review. |
| AM-REQ-009 | Require human approval for GPIO, electrical, KiCad, safety, and release decisions. | Signed/recorded review in `docs/release-evidence.md`. |
| AM-REQ-010 | Expose and independently control the nine named relay functions. | Approved relay inventory, wiring review, and per-output tests. |
| AM-REQ-011 | Expose 24 PWM outputs and three CCT fixtures. | Approved output inventory, channel-capacity analysis, and per-channel tests. |
| AM-REQ-012 | Implement ceiling-driver relay automation locally on the ESP32. | State-machine tests for on, off, restart, and maintenance modes. |
| AM-REQ-013 | Implement 12 V-driver relay automation locally on the ESP32. | State-machine tests covering all non-ceiling PWM outputs. |
| AM-REQ-014 | Implement stabilizer relay automation locally on the ESP32. | State-machine tests covering Desk 1/2/3 combinations and sequencing. |
| AM-REQ-015 | Provide explicit maintenance override modes for ceiling driver, 12 V driver, and stabilizer. | UX/API tests, persistence tests, and human safety review. |
| AM-REQ-016 | Keep local automations operational during Home Assistant, web, or remote disconnection. | Network-failure and recovery tests. |
| AM-REQ-017 | Define a future custom ESP remote command/state protocol with acknowledgement and reconciliation. | Protocol review and two-device integration test. |
| AM-REQ-018 | Use camera-conscious PWM frequencies and validate lighting with the team’s cameras. | Frequency/camera test matrix and recorded result. |
| AM-REQ-019 | Monitor current and voltage using the reviewed ADC1-based ZMCT103C/ZMPT101B architecture and derive load/energy health in software. | Electrical review, calibration evidence, software-calculation tests, and safe test result. |
| AM-REQ-020 | Provide deterministic boot, default-off, watchdog, safe-mode, OTA rollback, captive-portal recovery, network reconciliation, and boot recovery. | Power-cycle, brownout, watchdog, safe-mode, captive-portal, network-reconciliation, and OTA recovery tests. |
| AM-REQ-021 | Provide bounded persistent, Home Assistant-editable scenes through native-API services; YAML-only scenes are insufficient. | Persistent-store bounds, native-API scene service, restart, and power-loss tests. |

## Required local automation behavior

The following behavior is required in normal **AUTO** mode:

1. **Ceiling driver:** the ceiling relay is on when the ceiling light is logically on, meaning warm or cold PWM is requested above the approved off threshold. It is off when both channels are off. A small on/off hysteresis and debounce are required to prevent relay chatter.
2. **12 V driver:** the 12 V relay is on when any non-ceiling PWM output is active. It is off only when all non-ceiling PWM outputs are inactive. Hysteresis and a short settling delay are required.
3. **Voltage stabilizer:** the stabilizer relay is on when any of Desk 1, Desk 2, or Desk 3 is on. It may turn off only after all three desk relays are off for the approved delay. Desk outputs must not be presented as powered when the stabilizer is unavailable.
4. **Maintenance overrides:** each managed relay shall support explicit `AUTO`, `FORCE_ON`, and `FORCE_OFF` modes. `FORCE_OFF` is a safety-priority interlock and prevents automatic re-energization. Before a managed upstream relay opens, its dependent PWM or desk outputs shall be disabled and confirmed off, followed by the approved settling delay; opening under load without confirmation shall become a fault. `FORCE_ON` is for maintenance only, must be visibly indicated, and must not bypass hard electrical faults or emergency isolation. Override persistence, timeout, and release behavior require human approval before implementation.
5. **Network independence:** Home Assistant, web, and remote commands may change logical device state, but the ESP32 remains responsible for relay dependency evaluation and safe output enforcement.

## Scenes and persistent state

- Dynamic scenes shall be stored in a bounded persistent device store with documented size/count limits, versioning, and corruption handling. Home Assistant shall manage and activate scenes through callable ESPHome native-API services; YAML-only scene definitions are insufficient.
- Scene activation shall write desired state through the same validation, dependency, maintenance, and safety-interlock path as direct commands. A scene shall not bypass a hard fault, emergency isolation, or `FORCE_OFF`.
- The last valid desired state shall be persisted separately from transient physical output state. On boot or restart, outputs shall initialize safe/off, buses and sensors shall be initialized and validated, and faults shall be checked before any restoration.
- Only a valid desired state that remains safe under the current interlocks may be restored. Corrupt, incomplete, stale, or unsafe state shall be rejected; affected outputs shall remain off or inhibited and a fault shall be exposed.
- Energy, load, and sensor-health values shall be calculated in software from the existing current and voltage sensors. No dedicated metering IC is part of the approved architecture.
- ESP-NOW is deferred and is not an approved remote transport. The future remote remains a state-based, acknowledged protocol with pairing, sequence numbers, retries, replay protection, and reconciliation; its transport is TBD.

## Hardware and electrical TODOs

The following must be resolved before hardware-dependent implementation:

- ESP32 38-pin C-type USB development board, marking ETD11489, with 4 MB flash is confirmed; exact module/revision, board-level schematic, and physical pinout remain TODO.
- Exact GPIO and peripheral mapping: **TODO**; do not infer from a development board pin label or generic ESP32 pinout.
- Document the exact eight-channel relay-module mapping, every channel/load/rating, and the separate 30 A relay load/protection.
- Verify the selected 74HC595-family/74HCT595 relay-driver design: exact relay-module input current, active-LOW polarity, output-current margin, 3.3 V-to-5 V logic compatibility, the fixed `OE/G`-low and `SRCLR/MR`-high biases plus verified relay-input pull/default-off network, reset/brownout/register-corruption behavior, grounding, decoupling, and fault response.
- The Desk 1 and Desk 2 light-bar names are confirmed as separate fixtures; the two roof exhaust fans are confirmed to share one control signal and are connected in parallel. Verify shared fan input current, drive capability, wiring, and protection.
- Verify the ten ESP32-direct LEDC intents (six CCT channels plus four single-channel: Photo Spot, Wall Bounce, Washroom, Terrace) against the exact module’s LEDC capacity before GPIO allocation; route the remaining fourteen single-channel outputs through a single PCA9685 with two named backup channels (CH14/CH15). Do not invent GPIO assignments, PCA9685 addresses, or electrical implementation details.
- Verify the D4814 module datasheet, channel count, logic input voltage, PWM frequency range, current rating, thermal behavior, and input polarity.
- Verify the exact PCA9685 addresses, I2C topology, output-drive behavior, OE strategy, and power sequencing; a single 16-channel PCA9685 supports CH0–CH13 assigned and CH14/CH15 backup under the approved transport priority.
- Power input, regulators, current budget, inrush, protection, grounding, and thermal limits: **TODO**.
- Lighting and fan load topology, switching-device ratings, isolation, flyback/snubber requirements, and fail-safe state: **TODO**.
- Relay coil/input supply, opto-isolated module input-side grounding (a common ground is not inherently required, but the exact input circuit must be verified), 74HC595-family supply/level compatibility, and whether a shared low-voltage ground is required: **TODO**.
- Sensor models, interfaces, addresses, ranges, placement, calibration, and failure behavior: **TODO**.
- ADC1 measurement interface circuit, isolation/conditioning, calibration procedure, and qualified-person review: **TODO**.
- Indicator LEDs, display/OLED interface, buttons, enclosure, emergency disconnect, and environmental rating: **TODO**.
- Manufacturing, regulatory, and safety requirements: **TODO**.

## UX TODOs

- Indicator LED meanings and default states: **TODO**.
- OLED/status-display content and startup behavior: **TODO**.
- Locks, undo behavior, animations, and restart/recovery behavior: **TODO**.
- Local web UI authorization and safe control confirmation: **TODO**.

## Integration TODOs

- Home Assistant discovery/entity contract: **TODO**.
- Local web API routes, authentication, and failure responses: **TODO**.
- Offline behavior and state reconciliation: **TODO**.

## Recommended improvements

These are design recommendations for human review, not approved changes:

1. Validate the ADC1-based ZMCT103C/ZMPT101B measurement path with qualified electrical review, isolation/conditioning, calibration, and safe dummy-load testing; do not add a metering IC without an explicit reviewed design change.
2. Add relay contact or load-current feedback so AutoMate can detect a commanded-on output that did not actually energize.
3. Add input-voltage, regulator-temperature, enclosure-temperature, and brownout monitoring before unattended operation.
4. Use hardware default-off measures: a verified relay-input pull/default-off network (the ready relay modules provide their own internal input networks, so no external pull-ups are added), fixed `OE/G` tied low and `SRCLR/MR` tied high, a safe reset/brownout/register-corruption response, MOSFET gate pulldowns (omitted where the D4814 trigger module is fail-safe OFF), PCA9685 power-on/OE (the module straps OE LOW, so no ESP32 OE gate is used), and a defined boot sequence. The fixed biases alone do not prove passive default-off behavior.
5. Use high-frequency direct LEDC PWM for camera-critical CCT outputs and test every PWM source with the actual cameras, shutter speeds, and frame rates used by the team.
6. Add labeled terminal blocks, removable fuses, surge protection, test points, and an external emergency disconnect for commissioning and maintenance.
7. Consider smoke/heat and water-leak detection near the electrical enclosure and ceiling void as optional safety sensors.
8. Keep the future remote protocol state-based and acknowledged; never allow the remote to bypass the ESP32 safety/interlock layer.

## Non-goals

- Manufacturing-ready AI-generated schematics or PCBs without human review.
- Guessing GPIO assignments, electrical ratings, or safety limits.
- Committing secrets or customer data.
- Treating a candidate web interface as secure before threat and authorization review.

## Change control

Behavioral changes require a specification update, linked requirement/test IDs, documentation updates, and review of affected tests and release evidence. Hardware changes require an approved mapping and electrical/safety review.
