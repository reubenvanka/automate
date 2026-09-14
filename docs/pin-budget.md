# AutoMate GPIO Pin Budget (Approved Production Assignment)

> **Status**: APPROVED — GPIO + PWM transport allocation fixed for PCB, firmware, and schematic.
> **Source of Truth**: SPECIFICATIONS.md, PROJECT_PLAN.md, PCB_DESIGN_GUIDE.md
> **MCU**: ESP32-WROOM-32E (4MB Flash, 38-pin DevKit)
> **Last Updated**: 10 PWM on ESP32 (6 CCT + 4 single) + 14 on PCA9685 + 2 PCA9685 backup

---

## 1. ESP32 Pin Constraints (Hard Rules)

| Category | GPIOs | Reason |
|----------|-------|--------|
| Flash/PSRAM (internal) | GPIO6–11, GPIO16–17 | Connected to module flash/PSRAM — **never use** |
| UART0 (flash/debug) | GPIO1 (TX), GPIO3 (RX) | Keep free for flashing/serial debug — **never assign** |
| Input-only (GPI) | GPIO34, 35, 36, 37, 38, 39 | No output driver, no internal pull-up/down — **input only** |
| Strapping pins | GPIO0, 2, 4, 5, 12, 15 | Boot-mode pins — **no external pull-down/up**; usable as outputs only if no external pull |
| JTAG (avoid if debug needed) | GPIO12, 13, 14, 15 | JTAG pins — avoid if hardware debug needed |
| ADC2 (Wi-Fi conflict for ADC only) | GPIO0, 2, 4, 12, 13, 14, 15, 25, 26, 27 | ADC2 conflicts with Wi-Fi — **digital I/O/LEDC OK** |
| Not exposed (38-pin DevKit) | GPIO28–31 | Not broken out on board — **not assignable** |

---

## 2. Complete Production GPIO Assignment

| GPIO | Function | Type | Voltage | Notes |
|------|----------|------|---------|-------|
| **GPIO0** | — RESERVED / FREE — | — | 3.3V | Strapping — PCA9685 OE strapped LOW in module; no OE gate, no pull-up |
| **GPIO1** | UART0_TX (debug) | UART | 3.3V | **RESERVED** — do not assign |
| **GPIO2** | On-board LED / Status | Digital Out | 3.3V | On-board blue LED (active-LOW) |
| **GPIO3** | UART0_RX (debug) | UART | 3.3V | **RESERVED** — do not assign |
| **GPIO4** | DESK2_WARM (LEDC CH4) | LEDC PWM | 3.3V | Strapping — board must have no external pull |
| **GPIO5** | DESK2_COLD (LEDC CH5) | LEDC PWM | 3.3V | Strapping — board must have no external pull |
| **GPIO6–11** | FLASH/PSRAM | — | — | **DO NOT USE** — internal to ESP32 |
| **GPIO12** | PHOTO_SPOT_LIGHT (LEDC CH6) | LEDC PWM | 3.3V | Strapping/JTAG — no external pull |
| **GPIO13** | WALL_BOUNCE_LIGHT (LEDC CH7) | LEDC PWM | 3.3V | JTAG — avoid if debug needed |
| **GPIO14** | DOOR_SENSOR | Digital In | 3.3V | INPUT_PULLUP (MC-38 NO to GND) |
| **GPIO15** | 74HC595_RCLK | Digital Out | 3.3V | Shift register latch (strapping — no external pull) |
| **GPIO16–17** | FLASH | — | — | **DO NOT USE** — internal to ESP32 |
| **GPIO18** | CEILING_WARM (LEDC CH0) | LEDC PWM | 3.3V | Free GPIO |
| **GPIO19** | CEILING_COLD (LEDC CH1) | LEDC PWM | 3.3V | Free GPIO |
| **GPIO20** | 74HC595_SER | Digital Out | 3.3V | Shift register serial data |
| **GPIO21** | I²C_SDA | I²C | 3.3V | 4.7kΩ pull-up to 3.3V |
| **GPIO22** | I²C_SCL | I²C | 3.3V | 4.7kΩ pull-up to 3.3V |
| **GPIO23** | 74HC595_SRCLK | Digital Out | 3.3V | Shift register clock |
| **GPIO24** | NOT USABLE | — | — | Not exposed in ESP32 IO mux — DO NOT USE |
| **GPIO25** | DESK1_WARM (LEDC CH2) | LEDC PWM | 3.3V | ADC2/DAC — digital LEDC only |
| **GPIO26** | DESK1_COLD (LEDC CH3) | LEDC PWM | 3.3V | ADC2/DAC — digital LEDC only |
| **GPIO27** | TERRACE_LIGHT (LEDC CH8) | LEDC PWM | 3.3V | ADC2/DAC — digital LEDC only |
| **GPIO28–31** | NOT EXPOSED | — | — | Not broken out on 38-pin DevKit |
| **GPIO32** | RELAY_30A (direct) | Digital Out | 3.3V | Active-LOW; ready 30A module has internal input circuit |
| **GPIO33** | WASHROOM_LIGHT (LEDC CH9) | LEDC PWM | 3.3V | ADC1_CH4 |
| **GPIO34** | ZMCT103C_CURRENT (ADC1_CH6) | ADC1 Input | 3.3V | Input-only; conditioned CT input |
| **GPIO35** | ZMPT101B_VOLTAGE (ADC1_CH7) | ADC1 Input | 3.3V | Input-only; module output via 2:1 divider |
| **GPIO36** | — RESERVED — | — | 3.3V | Input-only; no INPUT_PULLUP support (34–39) |
| **GPIO37** | — RESERVED — | — | 3.3V | Input-only capable |
| **GPIO38** | — RESERVED — | — | 3.3V | Input-only capable |
| **GPIO39** | VS1838B_IR_RECEIVER | Digital In | 3.3V | Inverted in ESPHome (active-LOW output) |

---

## 3. ESP32 LEDC PWM Channel Summary (10 Channels)

> All use ESP32 LEDC hardware PWM at 10 kHz (camera-flicker-free, inaudible). No external pull-down required — the D4814 trigger modules are fail-safe OFF.

| LEDC CH | GPIO | Function | Type |
|---------|------|----------|------|
| CH0 | GPIO18 | CEILING_WARM | CCT pair |
| CH1 | GPIO19 | CEILING_COLD | CCT pair |
| CH2 | GPIO25 | DESK1_WARM | CCT pair |
| CH3 | GPIO26 | DESK1_COLD | CCT pair |
| CH4 | GPIO4 | DESK2_WARM | CCT pair |
| CH5 | GPIO5 | DESK2_COLD | CCT pair |
| CH6 | GPIO12 | PHOTO_SPOT_LIGHT | Single-channel |
| CH7 | GPIO13 | WALL_BOUNCE_LIGHT | Single-channel |
| CH8 | GPIO27 | TERRACE_LIGHT | Single-channel |
| CH9 | GPIO33 | WASHROOM_LIGHT | Single-channel |

> Note: the six CCT channels occupy the high-speed LEDC group (CH0–CH5) first, and each warm/cold pair is kept on adjacent GPIOs (18/19, 25/26, 4/5) for clean PCB routing. GPIO4/5 are strapping pins — board must have no external pull-down.
---

## 4. PCA9685 PWM Expander Assignment (I2C 0x40)

> Single 16-channel PCA9685 at I2C 0x40 (A0/A1 floating). Controlled over I2C (GPIO21 SDA, GPIO22 SCL). OE is strapped LOW in the module (outputs always enabled); no ESP32 OE gate and no GPIO allocated. CH0–CH13 assigned; CH14–CH15 are **kept as named backup channels** so future devices can be mapped without restructuring.

| PCA9685 CH | Function | Notes |
|------------|----------|-------|
| CH0 | MONITOR1_BACKLIGHT | Single-channel dimmable |
| CH1 | DESK_LIGHT1 | Single-channel dimmable |
| CH2 | TABLE_LIGHT | Single-channel dimmable |
| CH3 | SHELF_LIGHT1 | Single-channel dimmable |
| CH4 | MONITOR2_BACKLIGHT | Single-channel dimmable |
| CH5 | DESK_LIGHT2 | Single-channel dimmable |
| CH6 | SHELF_LIGHT2 | Single-channel dimmable |
| CH7 | CUPBOARD_BOUNCE_LIGHT | Single-channel dimmable |
| CH8 | CUPBOARD_SPOT_LIGHT | Single-channel dimmable |
| CH9 | DOOR_SPOT_LIGHT | Single-channel dimmable |
| CH10 | MONITOR3_BACKLIGHT | Single-channel dimmable |
| CH11 | MONITOR4_BACKLIGHT | Single-channel dimmable |
| CH12 | EXHAUST_FANS | Single-channel (shared signal for 2 fans) |
| CH13 | TERRACE_AMBIENT_LIGHT | Single-channel dimmable |
| CH14 | BACKUP_01 | **Backup — assigned, reserved for future device** |
| CH15 | BACKUP_02 | **Backup — assigned, reserved for future device** |

---

## 5. 74HC595 Shift Register (8-Channel Relay Module)

| 74HC595 Pin | ESP32 GPIO | Function |
|-------------|------------|----------|
| SER (Pin 14) | GPIO20 | Serial Data In |
| SRCLK (Pin 11) | GPIO23 | Shift Register Clock |
| RCLK (Pin 12) | GPIO15 | Register Clock (Latch) |
| OE/G (Pin 13) | GND | Tied LOW (always enabled) |
| SRCLR/MR (Pin 10) | 3.3V via 10kΩ | Tied HIGH (clear deasserted) |

| Relay Ch | Function | 74HC595 Q Output |
|----------|----------|------------------|
| CH1 | FAN | Q0 |
| CH2 | CEILING_LIGHTS_DRIVER | Q1 |
| CH3 | 12V_DRIVER | Q2 |
| CH4 | VOLTAGE_STABILIZER | Q3 |
| CH5 | DESK1 | Q4 |
| CH6 | DESK2 | Q5 |
| CH7 | DESK3 | Q6 |
| CH8 | 16A_RELAY | Q7 |

**30A Relay (direct)**: ESP32 GPIO32 (active-LOW; ready 30A module has internal input circuit)

**Manual override / bypass (force-ON only)**: Each relay channel has a DIP-switch position (SW1) that, when closed, pulls the relay input directly to GND — forcing the relay ON regardless of ESP32 state. A **1N5817 Schottky diode** in series between each 74HC595 output and the relay node isolates the shift-register output from the override short. Mapping is DIP position 1 → Relay CH1 … position 8 → Relay CH8. The 30A relay (GPIO32) gets its own NO push button (or one spare DIP position) from GPIO32 to GND. See `PCB_DESIGN_GUIDE.md` §4A for full pin-level instructions.

---

## 6. Sensor Interfaces

| Sensor | Interface | ESP32 Pin | Details |
|--------|-----------|-----------|---------|
| AHT20 (Temp/Humidity) | I²C (0x38) | GPIO21/22 | 4.7kΩ pull-ups |
| BMP280 (Pressure/Temp) | I²C (0x76/0x77) | GPIO21/22 | 4.7kΩ pull-ups |
| BH1750 (Light) | I²C (0x23) | GPIO21/22 | 4.7kΩ pull-ups |
| MC-38 Door Sensor | Digital | GPIO36 | INPUT_PULLUP, NO to GND |
| ZMCT103C Current CT | ADC1_CH6 | GPIO34 | Conditioned: burden 150Ω, bias divider, 10µF DC block, 3.3V Zener clamp |
| ZMPT101B Voltage | ADC1_CH7 | GPIO35 | Module output via 2:1 divider (2×10kΩ), 100nF filter, 3.3V Zener clamp |
| VS1838B IR Receiver | Digital | GPIO39 | Inverted in ESPHome (active-LOW output) |
---

## 7. PWM Channel Inventory (24 Active + 2 Backup)

| # | Function | Transport | Channel |
|---|----------|-----------|---------|
| 1 | CEILING_WARM | ESP32 LEDC | CH0 / GPIO18 |
| 2 | CEILING_COLD | ESP32 LEDC | CH1 / GPIO19 |
| 3 | DESK1_WARM | ESP32 LEDC | CH2 / GPIO25 |
| 4 | DESK1_COLD | ESP32 LEDC | CH3 / GPIO26 |
| 5 | DESK2_WARM | ESP32 LEDC | CH4 / GPIO4 |
| 6 | DESK2_COLD | ESP32 LEDC | CH5 / GPIO5 |
| 7 | PHOTO_SPOT_LIGHT | ESP32 LEDC | CH6 / GPIO12 |
| 8 | WALL_BOUNCE_LIGHT | ESP32 LEDC | CH7 / GPIO13 |
| 9 | WASHROOM_LIGHT | ESP32 LEDC | CH9 / GPIO33 |
| 10 | TERRACE_LIGHT | ESP32 LEDC | CH8 / GPIO27 |
| 11 | MONITOR1_BACKLIGHT | PCA9685 | CH0 |
| 12 | DESK_LIGHT1 | PCA9685 | CH1 |
| 13 | TABLE_LIGHT | PCA9685 | CH2 |
| 14 | SHELF_LIGHT1 | PCA9685 | CH3 |
| 15 | MONITOR2_BACKLIGHT | PCA9685 | CH4 |
| 16 | DESK_LIGHT2 | PCA9685 | CH5 |
| 17 | SHELF_LIGHT2 | PCA9685 | CH6 |
| 18 | CUPBOARD_BOUNCE_LIGHT | PCA9685 | CH7 |
| 19 | CUPBOARD_SPOT_LIGHT | PCA9685 | CH8 |
| 20 | DOOR_SPOT_LIGHT | PCA9685 | CH9 |
| 21 | MONITOR3_BACKLIGHT | PCA9685 | CH10 |
| 22 | MONITOR4_BACKLIGHT | PCA9685 | CH11 |
| 23 | EXHAUST_FANS | PCA9685 | CH12 (shared, 2 fans) |
| 24 | TERRACE_AMBIENT_LIGHT | PCA9685 | CH13 |
| — | BACKUP_01 | PCA9685 | CH14 (backup, reserved) |
| — | BACKUP_02 | PCA9685 | CH15 (backup, reserved) |

**Total: 10 ESP32 LEDC + 14 PCA9685 active = 24 PWM outputs, + 2 PCA9685 backup channels (CH14–CH15).**

---

## 8. Required External Components (Pull-ups / Pull-downs)

| Component | Value | Location | Purpose |
|-----------|-------|----------|---------|
| 1N5817 diode | 8× (1N5817) | Between each 74HC595 output and relay node | Manual-override isolation |
| 4.7kΩ pull-up | 4.7kΩ | GPIO21 (SDA) | I²C bus — keep only if modules lack built-in pull-ups |
| 4.7kΩ pull-up | 4.7kΩ | GPIO22 (SCL) | I²C bus — keep only if modules lack built-in pull-ups |
| 10kΩ pull-up | 10kΩ | 74HC595 SRCLR (Pin 10) | Bare shift-register control (clear deasserted) |
| 150Ω burden | 150Ω 1% | ZMCT103C CT secondary | Current-to-voltage |
| 2×10kΩ divider | 10kΩ 1% | ZMPT101B OUT | 2:1 voltage divider |
| 10µF electrolytic | 10µF 16V | ZMCT103C DC block | DC blocking |
| 100nF ceramic | 100nF | Multiple filter points | HF filtering |
| 3.3V Zener | BZX84C3V3 | ADC inputs (GPIO34, 35) | Overvoltage clamp |

> **No external pull-ups/pulldowns on module inputs:** the 8-ch + 30A relay modules provide their own input networks (internal pull-ups), the D4814 trigger modules are fail-safe OFF, and the PCA9685 module straps OE LOW (always enabled). Do not add external pull-ups/pulldowns on these.

---

## 9. Validation Checklist (Before PCB Order)

- [ ] Verify ESP32-WROOM-32E module marking matches pinout
- [ ] Confirm board has NO external pull-down/up on GPIO0,2,4,5,12,15
- [ ] Confirm UART0 (GPIO1/3) free for flashing/debug
- [ ] Verify 10 LEDC channels on ESP32 (GPIO 4,5,12,13,18,19,25,26,27,33) all use 10kHz, 10kΩ pull-downs
- [ ] Confirm PCA9685 single device: CH0–13 assigned, CH14/15 = BACKUP_01/02
- [ ] Verify 1N5817 diode orientation: anode to 74HC595, cathode to relay node (8×)
- [ ] Confirm DIP switch (SW1) position order: position 1 → Relay CH1 … position 8 → Relay CH8
- [ ] Confirm DIP switch low-side pins (positions 1–8) tied to GND
- [ ] Measure ZMCT103C burden resistor value with 5A/10A loads
- [ ] Verify ZMPT101B module output range (0–5V or 0–3.3V)
- [ ] Confirm MC-38 door sensor NO/NC wiring
- [ ] Validate 74HC595 vs 74HCT595 choice (3.3V logic)
- [ ] Confirm PCA9685 address pins (A0/A1) for 0x40
- [ ] Check LEDC channel availability (10 channels needed)
- [ ] Verify all strapping pins safe for assigned PWM use
- [ ] Confirm 10kΩ pull-down/up resistor footprints on PCB
- [ ] Validate I²C bus length < 30cm, proper pull-ups
- [ ] Confirm 30A relay module 3.3V logic compatibility

---

## 10. Change Log

| Date | Change |
|------|--------|
| 2024 | Fresh production GPIO assignment from scratch |
| 2024 | Transport locked: 10 ESP32 LEDC (6 CCT + 4 single) + 14 PCA9685 + 2 backup |
| 2024 | Removed CEILING_COLD duplication and GPIO17 (flash) misuse |
| 2024 | PCA9685 CH14/CH15 named BACKUP_01/BACKUP_02 (reserved, not unassigned) |
| 2024 | Relay manual override/bypass with 8× 1N5817 diodes documented |
| 2024 | Validation checklist added |

---

*This document is the single source of truth for PCB layout and firmware GPIO configuration.*