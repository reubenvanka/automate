# AutoMate Architecture

**Source:** `SPECIFICATIONS.md` — update the specification before changing architecture.

## Product boundary

AutoMate is the local controller for the Home Office lighting, fan, desk-power, stabilizer, appliance, environmental, door, current, and voltage systems. Home Assistant, the authenticated local web interface, and the future custom ESP remote are control clients; they must not bypass the ESP32 control-state, interlock, or safety layers.

## Architectural principles

1. **Local autonomy:** relay dependency and PWM safety logic run on the ESP32 without Home Assistant, Wi-Fi, or a web client.
2. **One state model:** every client writes desired state to one validated control-state model; physical outputs are derived from that model plus maintenance and safety interlocks.
3. **Safe defaults:** outputs start off, active-LOW relay inputs are held inactive during boot through a verified pull/default-off network, the 74HC595-family `OE/G` pin is tied low and `SRCLR/MR` is tied high, the separate 30 A relay GPIO has a verified default state, and MOSFET gates are held off. The fixed register biases consume no ESP32 GPIOs and do not by themselves prove passive default-off behavior; PCA9685 output-enable/default behavior and power sequencing remain TODO.
4. **Explicit overrides:** `AUTO`, `FORCE_ON`, and `FORCE_OFF` are visible, auditable modes for managed relays; `FORCE_OFF` has safety priority.
5. **No guessed hardware:** GPIO numbers, relay channels, MOSFET-module behavior, load ratings, and mains connections remain TODO until approved.
6. **Camera-conscious lighting:** ten ESP32 hardware PWM/LEDC channels (six CCT plus four single-channel: Photo Spot, Wall Bounce, Washroom, Terrace) have direct priority; the remaining fourteen single-channel outputs use PCA9685 plus two backup channels. Software or bit-banged PWM is prohibited, and actual lighting behavior must be tested with the team’s cameras before release.

## Firmware layers

### Product assembly

`firmware/esphome/studio-controller.yaml` (final name TBD) will assemble focused ESPHome packages for core networking, outputs, relays, PWM, sensors, state management, automation, web control, diagnostics, and future remote communication.

### Control-state and validation layer

- Stable logical IDs for every relay, PWM channel, CCT fixture, sensor, lock, and maintenance mode.
- Command validation for range, type, device identity, source authorization, and state transitions.
- Desired state versus effective state, so a commanded output can be blocked by a relay interlock or maintenance mode.
- Bounded persistent storage for the last valid desired state and Home Assistant-managed scenes, with documented size/count limits, versioning, corruption handling, and native-API services; YAML-only scene definitions are insufficient.
- Safe startup separates persisted desired state from transient physical output state: outputs initialize off, buses and sensors initialize and validate, and faults are checked before any restoration.
- Only a valid desired state that remains safe under current interlocks may be restored; corrupt, incomplete, stale, or unsafe state is rejected, affected outputs remain off or inhibited, and a fault is exposed.
- Central dependency engine for ceiling-driver, 12 V-driver, and stabilizer relays.

### Output layer

- **Relay outputs:** one 8-bit 74HC595-family register drives the eight-channel active-LOW relay-module inputs; the separate 30 A relay input uses one separate ESP32 GPIO, not a second register. The register outputs drive the module input circuitry, never the relay coils. The ESP32 controls the register through three signals only: serial data (`SER`), shift clock (`SRCLK`), and storage-register/latch clock (`RCLK`). `OE/G` is tied low and `SRCLR/MR` is tied high; these fixed biases consume no ESP32 GPIOs. Tying `OE/G` low enables the register outputs and tying `SRCLR/MR` high deasserts clear; neither bias by itself proves that the relay inputs are off. A verified relay-input pull/default-off network and reset/brownout/register-corruption tests are required. The direct GPIO and register path must each pass logic-level, input-current, and default-off tests. Relay feedback is recommended.
- **Direct PWM:** Ten ESP32 hardware PWM/LEDC channels (six CCT plus four single-channel: Photo Spot, Wall Bounce, Washroom, Terrace). The exact ESP32 module on the ETD11489 board is unverified, so the ten direct intents must be checked against the exact module's LEDC capacity before GPIO allocation. Software or bit-banged PWM is prohibited.
- **Expanded PWM:** The remaining fourteen single-channel outputs use a single 16-channel PCA9685 (CH0–CH13), with CH14/CH15 kept as named backup channels. Exhaust Fans is one shared PCA9685 control output for two roof fans connected in parallel; independent fan speed control is not part of the confirmed topology. The exact address, I2C topology, output-drive behavior, OE strategy, and power sequencing remain TODO. PCA9685 outputs must drive the inputs of approved output-driver modules, never lighting loads directly. The shared fan signal's input-current, drive, wiring, and protection requirements remain TODO.
- **CCT mixing:** warm and cold channels are controlled as a pair with brightness, color-temperature, gamma, minimum-off threshold, and smooth transitions.

### Automation layer

- Ceiling relay follows the logical ceiling-light state.
- 12 V relay follows any active non-ceiling PWM output.
- Stabilizer relay follows any active Desk 1, Desk 2, or Desk 3 relay.
- Hysteresis, debounce, sequencing delays, and relay-chatter prevention are part of the control algorithm.
- Maintenance modes gate physical outputs without allowing a client to bypass hard faults or emergency isolation. A managed relay force-off first disables and confirms its dependent loads, waits for the approved settling delay, and faults if feedback is unavailable or disagreeing.

### Interface layer

- **Home Assistant:** ESPHome native API with stable entity IDs, availability states, services/actions, persistent scene services, and secret-safe configuration.
- **Local web:** authenticated ESPHome web-server/API surface for approved controls and status; custom UI/API behavior requires threat and authorization review.
- **Future ESP remote:** ESP-NOW is deferred and is not an approved transport. A future state-based, acknowledged protocol with sequence numbers, retries, pairing, replay protection, and periodic state reconciliation remains required; transport selection is TBD.
- **Local fallback:** when upstream clients disconnect, the ESP32 continues local automation and exposes only approved local controls.

### Sensor and diagnostics layer

- AHT20 and BMP280 for environmental data; BH1750 for illuminance.
- MC-38 door/window input with configurable NO/NC polarity and debounce.
- ADC1-based ZMCT103C and ZMPT101B measurement is the selected direction, but only after an approved isolated/conditioned interface and calibration procedure.
- Load, energy, and sensor-health monitoring is derived in software from the reviewed current/voltage measurements; no dedicated metering IC is planned.
- I2C device health, ADC sanity checks, relay feedback, supply voltage, brownout, watchdog, temperature, and communication diagnostics.

<!-- ARCHITECTURE_CONTINUATION -->

## Hardware architecture

### Controller and I/O budget

The controller is an ESP32 38-pin C-type USB development board marked ETD11489 with 4 MB flash. The underlying ESP32 module/revision and board-level schematic still require physical verification. The pin-allocation exercise must account for:

- three ESP32-controlled register signals—serial data (`SER`), shift clock (`SRCLK`), and storage-register/latch clock (`RCLK`)—with `OE/G` tied low and `SRCLR/MR` tied high; these fixed biases consume no GPIOs, and the separate 30 A relay adds one GPIO for a fixed relay-control budget of four GPIOs;
- nine relay functions across one eight-channel module plus one separate 30 A relay; no second shift register is planned;
- ten ESP32 hardware PWM/LEDC channels (six CCT plus four single-channel) and the remaining fourteen single-channel outputs on PCA9685 plus two backup channels; software/bit-banged PWM is prohibited;
- I2C SDA/SCL for PCA9685 and sensors;
- two ADC1 analog inputs for conditioned current/voltage signals;
- door input, status indicators, buttons, and future remote interface;
- module-flash pins, input-only pins, ADC2/Wi‑Fi restrictions, strapping pins, UART flashing/debug pins, and external pull requirements, all verified against the exact module and board schematic.

No final GPIO map is approved by this document.

### Power and load topology

- Separate, fused, and appropriately regulated logic, relay-coil, 12 V lighting, and sensor supplies.
- Common grounding and isolation boundaries documented per module; optocoupler isolation does not inherently require a common ground, but the exact relay-module input circuit and return-current path require verification.
- MOSFET modules used only within their verified voltage/current/thermal limits, with gate pulldowns, input protection, and flyback/snubber protection for inductive loads.
- Relay contact ratings, continuous-load derating, inrush, arc suppression, wire gauge, terminals, enclosure temperature, and mains clearance/creepage approved by a qualified reviewer.
- External emergency disconnect and labeled test points recommended before commissioning.

### Sensor architecture

- Keep I2C wiring short and noisy-load separated where possible; use verified pull-ups, address assignment, bus scanning, and timeout handling.
- Treat raw AC sensor modules as unsafe until their isolation, burden, biasing, ADC range, filtering, and calibration are approved.
- Use the selected ADC1-based ZMCT103C/ZMPT101B path only after qualified review of isolation, conditioning, ADC range, filtering, and calibration; no dedicated metering IC is planned.
- Treat the 74HC595-family register path and separate 30 A relay GPIO as unapproved until relay-input current, 3.3 V-to-5 V logic compatibility, output-current margin, the fixed `OE/G`-low/`SRCLR/MR`-high biases, external relay-input default-off network, reset/brownout/register-corruption behavior, grounding, and fault tests pass. The fixed biases alone do not prove passive default-off behavior.

## Security and recovery

- Wi-Fi, API, OTA, and web credentials remain in ignored local secret files or an external secret store.
- Home Assistant API encryption and local web authentication are required.
- The local web interface is not exposed to the public internet without a reviewed threat model, secure transport/proxy strategy, authorization, rate limiting, and CSRF/session controls.
- Remote pairing keys and replay-protection material are never committed to Git.
- Logs avoid credentials, tokens, and unnecessary personal data; maintenance and safety events are auditable.
- Boot initializes outputs to a safe/off state before network services, then initializes and validates buses, sensors, and control state.
- Reconnect retains local automation and reconciles client state without blindly re-energizing unsafe loads.
- Brownout/reset enters a defined safe state, records the event, and follows the approved restoration policy.
- The last valid desired state and Home Assistant-managed scenes are stored separately from transient physical output state. After boot/restart/power loss, only valid state that remains safe under current interlocks may be restored; corrupt, incomplete, stale, or unsafe state is rejected and exposed as a fault.
- Sensor or output disagreement inhibits automatic re-energization and exposes a fault.
- Watchdog, safe-mode recovery, OTA rollback, post-update smoke tests, scene persistence, and recovery/restart tests are required.

## Validation strategy

1. ESPHome YAML validation and firmware build.
2. Unit tests for command validation, CCT mixing, dependency logic, overrides, hysteresis, scene persistence/validation, software-derived diagnostics, and recovery.
3. Hardware-in-the-loop tests for every relay and PWM channel using safe dummy loads, including verification that software/bit-banged PWM is not used.
4. Camera flicker tests at the team’s actual camera settings.
5. Network disconnect, web/HA restart, future remote packet loss after transport selection, brownout, watchdog, restart, power-loss restoration, and OTA rollback tests.
6. Sensor calibration and failure injection, including load/energy/sensor-health diagnostics.
7. KiCad netlist, ERC, DRC, thermal/electrical review, and human sign-off.
8. Release evidence linked to `AM-REQ-001` through `AM-REQ-021`; ESP-NOW remains deferred and is not approved as the remote transport.
