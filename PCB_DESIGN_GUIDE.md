# AutoMate PCB Design Guide

> **Status**: Production Design Guide — Aligned with pin-budget.md GPIO assignment
> **MCU**: ESP32-WROOM-32E (4MB Flash, 38-pin DevKit)
> **Last Updated**: Production design — ready for schematic capture and PCB layout

---

## 1. Pin Assignment Map (Production)

| GPIO | Function | Type | Voltage Domain | Notes |
|------|----------|------|----------------|-------|
| **GPIO0** | 74HC595_SER | Digital Out | 3.3V | Shift register serial data (strapping/BOOT — no external pull) |
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
| **GPIO20** | NOT ON DEVKIT HEADER | — | — | Valid on chip but not broken out on the DevKit — DO NOT USE |
| **GPIO21** | I²C_SDA | I²C | 3.3V | 4.7kΩ pull-up (verify module) |
| **GPIO22** | I²C_SCL | I²C | 3.3V | 4.7kΩ pull-up (verify module) |
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
| **GPIO37** | — RESERVED — | — | 3.3V | Input-only capable — available for future use |
| **GPIO38** | — RESERVED — | — | 3.3V | Input-only capable — available for future use |
| **GPIO39** | VS1838B_IR_RECEIVER | Digital In | 3.3V | Inverted in ESPHome (active-LOW output) |

---

## 2. ESP32 LEDC PWM Outputs (D4814 fail-safe)

No external pull-down required — the D4814 trigger modules are fail-safe OFF.

| GPIO | Function | Note |
|------|----------|------|
| GPIO18 | CEILING_WARM (LEDC CH0) | — |
| GPIO19 | CEILING_COLD (LEDC CH1) | — |
| GPIO25 | DESK1_WARM (LEDC CH2) | — |
| GPIO26 | DESK1_COLD (LEDC CH3) | — |
| GPIO4 | DESK2_WARM (LEDC CH4) | strapping — no external pull |
| GPIO5 | DESK2_COLD (LEDC CH5) | strapping — no external pull |
| GPIO12 | PHOTO_SPOT_LIGHT (LEDC CH6) | strapping/JTAG |
| GPIO13 | WALL_BOUNCE_LIGHT (LEDC CH7) | JTAG |
| GPIO27 | TERRACE_LIGHT (LEDC CH8) | — |
| GPIO33 | WASHROOM_LIGHT (LEDC CH9) | ADC1_CH4 |

> Note: CCT channels occupy the high-speed LEDC group (CH0–CH5) with warm/cold pairs on adjacent GPIOs (18/19, 25/26, 4/5) for clean PCB routing. GPIO4/5 are strapping pins — board must have no external pull-down.

Frequency: 10 kHz (camera-flicker-free, inaudible)

---

## 3. PCA9685 PWM Expander (I2C 0x40)

| Parameter | Value |
|-----------|-------|
| I2C Address | 0x40 (A0=A1=floating) |
| I2C Bus | GPIO21 (SDA), GPIO22 (SCL) @ 50 kHz |
| OE | Strapped LOW in module (outputs always enabled); no ESP32 gate |
| VCC | 5V (shared with relay module) |
| Output Channels | 16 (CH0–CH15) |
| Frequency | 10 kHz (camera-flicker-free) |
| Output Type | Open-drain, 5V tolerant |

PCA9685 Channel Map:

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
| CH14 | BACKUP_01 | Backup — reserved for future device |
| CH15 | BACKUP_02 | Backup — reserved for future device |

Note: OE is strapped LOW in the module (outputs always enabled). No ESP32 OE gate, no pull-up, and no GPIO allocated for OE.


---

## 4. 74HC595 Shift Register (8-Channel Relay Module)

### Connections

| 74HC595 Pin | ESP32 GPIO | Function |
|-------------|------------|----------|
| SER (Pin 14) | GPIO0 | Serial Data In |
| SRCLK (Pin 11) | GPIO23 | Shift Register Clock |
| RCLK (Pin 12) | GPIO15 | Register Clock (Latch) |
| OE/G (Pin 13) | GND | Tied LOW (always enabled) |
| SRCLR/MR (Pin 10) | 3.3V via 10kΩ | Tied HIGH (clear deasserted) |

### Relay Channel Map

| Relay Ch | Function | 74HC595 Q Output |
|----------|----------|------------------|
| CH1 | FAN | Q0 |
| CH2 | CEILING_LIGHTS_DRIVER | Q1 |
| CH3 | 12V_DRIVER | Q2 |
| CH4 | VOLTAGE_STABILIZER | Q3 |
| CH5 | DESK1 | Q4 |
| CH6 | DESK2 | Q5 |
| CH6 | DESK3 | Q6 |
| CH7 | 16A_RELAY | Q7 |

30A Relay (direct): ESP32 GPIO32 (active-LOW; ready 30A module has internal input circuit)

Important: 74HC595 powered at 3.3V (not 5V) to match ESP32 logic levels. Relay module must accept 3.3V logic (active-LOW). The ready relay modules provide their own input networks (internal pull-ups to module VCC for default-OFF); no external pull-ups are added.

### 4A. Manual Override / Bypass (Force-ON Only)

A manual override lets an operator energize any relay regardless of ESP32 state (dead, powered-down, or actively commanding OFF). This supports the "maintenance overrides" requirement in confirmed scope item 9. It provides **force-ON only**; it does not force a relay OFF while the ESP32 is driving it ON, and it does not remove the active-LOW default-off pull.

**Circuit per relay channel:**

```
74HC595 Qx ──┬── [1N5817 diode, anode →] ──┬── relay input (J5-Py)
              │                              │
             GND                    DIP switch (SW1, one position)
                                          │  (switch closed = override ON)
                                     SW1-Pz ── GND
```

- **1N5817 Schottky diode** (anode to 74HC595, cathode to relay node): isolates the shift-register output when the DIP override is closed (reverse-biased → no short). When the override is open it conducts with a ~0.2V drop, so normal ESP32 control is unaffected.
- **DIP switch** (positions of the existing SW1 12-position bank): closed = relay node pulled to GND → relay forced ON regardless of 74HC595 state. Open = relay follows the 74HC595.

**Reasoning (do not substitute a 1kΩ resistor):** a series resistor forms a voltage divider with the relay-module pull-up and can leave the relay input above the optocoupler/LED turn-on threshold when the 74HC595 goes LOW, preventing reliable relay ON. The diode has no such divider problem.

**DIP position → Relay mapping (position 1 = Relay 1 … position 8 = Relay 8):** the DIP switch pins are numbered 1–24 where pin **n** pairs with pin **n+12** as one slide position. The relay node connects to the high pin (n+12); the low pin (n) ties to GND. The existing schematic currently wires the high pins in descending order (pin 24 → CH1 … pin 17 → CH8), which is reversed; it must be re-assigned so position 1 drives CH1.

| DIP position | Relay CH | 74HC595 U2 pin | Relay J5 pin | DIP high pin (→ relay node) | DIP low pin (→ GND) |
|--------------|----------|----------------|--------------|-----------------------------|---------------------|
| 1 | CH1 | U2-15 | J5-2 | SW1-13 | SW1-1 |
| 2 | CH2 | U2-1 | J5-3 | SW1-14 | SW1-2 |
| 3 | CH3 | U2-2 | J5-4 | SW1-15 | SW1-3 |
| 4 | CH4 | U2-3 | J5-5 | SW1-16 | SW1-4 |
| 5 | CH5 | U2-4 | J5-6 | SW1-17 | SW1-5 |
| 6 | CH6 | U2-5 | J5-7 | SW1-18 | SW1-6 |
| 7 | CH7 | U2-6 | J5-8 | SW1-19 | SW1-7 |
| 8 | CH8 | U2-7 | J5-9 | SW1-20 | SW1-8 |

**Schematic changes to implement (human-reviewed pass required per workspace rules):**

1. **Insert 8× 1N5817** — for each relay CH (table above), place a diode between the U2 output pin and the existing relay node (J5-Py / DIP high pin). Diode **anode** on the U2 pin, **cathode** on the relay node. This breaks the current direct U2→relay connection.
2. **Re-assign the DIP wiring to position order** — connect each relay node to the high pin per the table (pos 1=CH1 … pos 8=CH8), and connect all low pins to **GND** (positions 1–8 use SW1-1 .. SW1-8). Note the current schematic's descending assignment (pin 24→CH1 … 17→CH8) must be corrected.
3. **30A relay (GPIO32) bypass** — add a separate NO push button from GPIO32 to GND (momentary force-ON), or use one spare SW1 position similarly (one side to GPIO32, other to GND). GPIO32 is active-LOW; the ready 30A module has its own internal input circuit.
4. **Spare DIP positions** — SW1 positions 9–12 (pins 9/21, 10/22, 11/23, 12/24) remain unused; reserve for the 30A relay, future channels, or leave floating.

**Pull-up note:** the relay module (optocoupler board) already contains its own input pull-up resistors, so external 10kΩ pull-ups on the relay channels are redundant. Confirm by measuring input-to-VCC resistance on the purchased module; remove the external ones if present to avoid divider interference.

---

## 5. Power Architecture

### Power Rails

| Rail | Source | Consumers |
|------|--------|-----------|
| 5V | USB-C / onboard regulator | ESP32 (via 3.3V reg), 74HC595, Relay module, ZMPT101B, PCA9685 |
| 3.3V | ESP32 onboard LDO | ESP32 I/O, PCA9685 pull-ups, 30A relay pull-up, ZMCT103C bias, ADC protection |
| 12V | Separate supply | 12V LED drivers (via relay AM-RLY-03) |
| Mains | Isolated | ZMPT101B AC terminals, ZMCT103C primary (through load wire) |

### Decoupling (place near each IC)
- ESP32: 10µF + 100nF on 3.3V, 10µF + 100nF on 5V
- 74HC595: 100nF on pin 16 (VCC)
- PCA9685: 10µF + 100nF on VCC
- ZMPT101B: 10µF + 100nF on VCC
- Relay module: 100nF on 5V input

---

## 6. I2C Bus

- SDA: GPIO21 → 4.7kΩ pull-up to 3.3V → PCA9685, AHT20, BMP280, BH1750
- SCL: GPIO22 → 4.7kΩ pull-up to 3.3V → same devices
- Bus length: Keep <30cm, route as differential pair if possible
- Speed: 50 kHz


---

## 7. Sensor Interfaces

### ZMCT103C Current Transformer (GPIO34 / ADC1_CH6)

- Burden resistor: 150Ω 1% (for 10A max, 5mA at 5A → ~0.75V peak at 10A)
- Bias divider: 2× 10kΩ 1% (3.3V → 1.65V center)
- DC blocking: 10µF 16V electrolytic + 100nF ceramic
- Protection: 1kΩ series + BZX84C3V3 Zener clamp to GND
- Calibration: 1.65V = 0A, adjust linear calibration points

### ZMPT101B Voltage Sensor (GPIO35 / ADC1_CH7)

- Module supply: 5V
- AC input: Live + Neutral (250V max)
- Output: 0–5V proportional to RMS voltage
- Divider: 2× 10kΩ 1% (5V → 2.5V max at ADC)
- Filter: 100nF ceramic from GPIO35 to GND
- Protection: BZX84C3V3 Zener clamp to GND
- Calibration: Linear (0V→0V, 1.25V→125V, 2.5V→250V)

### Door Sensor (MC-38)
- GPIO36 (input-only)
- Wiring: NO contact → GPIO36, COM → GND
- ESPHome: pin: { number: 36, mode: INPUT_PULLUP, inverted: true }

### IR Receiver (VS1838B)
- GPIO39 (input-only)
- ESPHome: remote_receiver: { pin: { number: 39, inverted: true }, dump: all }

### Environmental Sensors (I2C)
- AHT20: 0x38, GPIO21/22
- BMP280: 0x76/0x77, GPIO21/22
- BH1750: 0x23, GPIO21/22

---

## 8. PWM Summary

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


---

## 9. Required External Components

| Component | Value | Location | Purpose |
|-----------|-------|----------|---------|
| 1N5817 diode | 8× (1N5817) | Between each 74HC595 output and relay node | Manual-override isolation (anode to 74HC595) |
| 4.7kΩ pull-up | 4.7kΩ | GPIO21 (SDA) | I2C bus — keep only if modules lack built-in pull-ups |
| 4.7kΩ pull-up | 4.7kΩ | GPIO22 (SCL) | I2C bus — keep only if modules lack built-in pull-ups |
| 10kΩ pull-up | 10kΩ | 74HC595 SRCLR (Pin 10) | Bare shift-register control (clear deasserted) |
| 150Ω burden | 150Ω 1% | ZMCT103C CT secondary | Current-to-voltage |
| 2×10kΩ divider | 10kΩ 1% | ZMPT101B OUT | 2:1 voltage divider |
| 10µF electrolytic | 10µF 16V | ZMCT103C DC block | DC blocking |
| 100nF ceramic | 100nF | Multiple filter points | HF filtering |
| 3.3V Zener | BZX84C3V3 | ADC inputs (GPIO34, 35) | Overvoltage clamp |

> **No external pull-ups/pulldowns on module inputs:** the 8-ch + 30A relay modules provide their own input networks (internal pull-ups), the D4814 trigger modules are fail-safe OFF, and the PCA9685 module straps OE LOW (always enabled). Do not add external pull-ups/pulldowns on these.

---

## 10. Validation Checklist (Before PCB Order)

- [ ] Verify ESP32-WROOM-32E module marking matches pinout
- [ ] Confirm board has NO external pull-down/up on GPIO0,2,4,5,12,15
- [ ] Confirm UART0 (GPIO1/3) free for flashing/debug
- [ ] Measure ZMCT103C burden resistor value with 5A/10A loads
- [ ] Verify ZMPT101B module output range (0–5V or 0–3.3V)
- [ ] Confirm MC-38 door sensor NO/NC wiring
- [ ] Validate 74HC595 vs 74HCT595 choice (3.3V logic)
- [ ] Verify 1N5817 diode orientation: anode to 74HC595, cathode to relay node (8×)
- [ ] Confirm DIP switch (SW1) position order: position 1 → Relay CH1 … position 8 → Relay CH8
- [ ] Confirm DIP switch low-side pins (positions 1–8) tied to GND
- [ ] Confirm 30A relay bypass (NO push button or spare DIP position to GND)
- [ ] Confirm PCA9685 address pins (A0/A1) for 0x40
- [ ] Check LEDC channel availability (10 channels needed)
- [ ] Verify all strapping pins safe for assigned PWM use
- [ ] Confirm NO external pull-up/pulldown added on ready module inputs (PCA9685 OE, relay + 30A modules, D4814 triggers)
- [ ] Validate I2C bus length < 30cm, proper pull-ups
- [ ] Confirm 30A relay module 3.3V logic compatibility

---

## 11. Revision History

| Date | Revision | Description |
|------|----------|-------------|
| 2024-01-15 | 1.0 | Initial production design guide |
| 2024-01-20 | 1.1 | Updated GPIO map per fresh production assignment |
| 2024-01-22 | 1.2 | Finalized 10 ESP32 LEDC + 14 PCA9685 PWM channels |
| 2024-01-24 | 1.3 | Added relay manual override/bypass (§4A): 8× 1N5817 diodes, DIP position 1→Relay CH1 order, DIP low-pin GND tie, 30A relay bypass |
| 2024-01-25 | 1.4 | Corrected PWM transport per SPECIFICATIONS.md: 10 ESP32 LEDC (6 CCT on high-speed CH0–5 + Photo Spot/Wall Bounce/Washroom/Terrace) + 14 PCA9685 CH0–13 + BACKUP_01/02 (CH14–15); removed CEILING_COLD duplication and GPIO17 misuse |
| 2024-01-26 | 1.5 | Removed external pull-up/pulldown networks on ready modules (PCA9685 OE strapped LOW = always enabled; 8-ch + 30A relay modules have internal input circuits; D4814 trigger modules fail-safe OFF); corrected Section 1 GPIO map to the approved CCT-grouped allocation |

---

*This document and pin-budget.md are the authoritative sources for schematic capture and PCB layout.*


