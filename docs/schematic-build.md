# AutoMate Schematic Build Checklist (KiCad GUI)

> **Purpose:** Step-by-step guide to assemble the `AutoMate_Rebuild` schematic in the KiCad GUI.
> **Method:** Option B — user places component symbols and draws pin-to-pin wires in KiCad; AI added wires/labels/validation are limited by the MCP server not resolving symbol libraries, so connectivity is drawn in the GUI and validated afterward with netlist + ERC/DRC.
> **Source of truth:** SPECIFICATIONS.md, docs/pin-budget.md, PCB_DESIGN_GUIDE.md
> **Blocked on:** symbol-library import (see §0) before components can be placed.

---

## 0. One-time symbol-library setup (before placing parts)

The MCP toolchain and KiCad need the parts registered as KiCad symbol/footprint libraries. Parts downloaded from the registry live in `/Users/reubenvanka/Documents/iot-workspace/KiCad_Assets/` as `.kicad_sym` / `.kicad_mod` / `.step` but are **not yet imported**.

1. Open KiCad → Schematic editor → **Add Symbol** (or the symbol browser).
2. Import each `.kicad_sym` into a symbol library (e.g., a new `AutoMate` library):
   - `KiCad_Assets/1N5817/1N5817.kicad_sym`
   - `KiCad_Assets/ESP32-DEVKITC/MODULE_ESP32-DEVKITC.kicad_sym`
   - `KiCad_Assets/74HC595N_112/74HC595N_112.kicad_sym`
3. Ensure a `sym-lib-table` for the project references these so they resolve on the sheet.

---

## 1. Component placement (suggested grid — A3 landscape)

| Ref | Component | Suggested (x, y) mm | Library : Symbol |
|-----|-----------|---------------------|------------------|
| U1 | ESP32-WROOM-32E DevKit | (80, 150) | `snapeda:MODULE_ESP32-DEVKITC` |
| U2 | 74HC595 shift register | (165, 145) | `74xx:74HC595` |
| D1–D8 | 1N5817 Schottky diodes | (150–200, 95) | `AutoMate:1N5817` (imported) |
| SW1 | 12-position DIP switch | (165, 70) | `Switch:SW_DIP_x12` |
| J5 | Relay connector (1×10) | (230, 120) | `Connector:Conn_01x10_Socket` |
| J2_PCA9685 | PCA9685 connector (1×6) | (60, 210) | `Connector:Conn_01x06_Socket` |
| J11 | Door sensor conn (1×2) | (115, 230) | `Connector:Conn_01x02_Socket` |
| J9 | ZMPT conn (1×4) | (150, 230) | `Connector:Conn_01x04_Socket` |
| VoltageSen1 | Voltage sensor conn (1×3) | (185, 230) | `Connector:Conn_01x03_Socket` |
| J1 | RJ45 / cable conn | (230, 190) | `8P8C` (RJ45) |
| AC1 | AC input | (260, 210) | `Conn_01x03_Socket` |
| T1–T6 | Screw terminals | (230–280, 230) | `Screw_Terminal_01x02` |
| F1 | Fuse | (245, 200) | `Polyfuse` |
| M1–M10 | Dual MOSFET modules | lower band | `00.snapeda:D4184 Module Custom Updated` |

*(Coordinates are suggestions; place symbols by clicking in KiCad. Only SW1/U2/J5/D1–D8 positions matter for wiring below.)*

---

## 2. Wiring / connectivity (pin-by-pin)

### 2.1 ESP32 → sensors / outputs (GPIO map)

Wire each ESP32 GPIO leg to its destination. Save label net names per `docs/pin-budget.md`.

| From (ESP32 pin) | To | Net name (label) |
|------------------|----|------------------|
| GPIO0 | — (free — no OE gate; module strapped low) | — |
| GPIO0 | U2 SER | `SER` |
| GPIO23 | U2 SRCLK | `SRCLK` |
| GPIO15 | U2 RCLK | `RCLK` |
| GPIO21 | J2_PCA9685 + sensors SDA | `I2C_SDA` |
| GPIO22 | J2_PCA9685 + sensors SCL | `I2C_SCL` |
| GPIO34 | ADC1 (ZMCT103C) | `ZMCT103C_CURRENT` |
| GPIO35 | ADC1 (ZMPT101B) | `ZMPT101B_VOLTAGE` |
| GPIO14 | door sensor (J11) | `DOOR_SENSOR` |
| GPIO39 | IR receiver | `IR_RX` |
| GPIO32 | 30A relay bypass | `RELAY_30A` |

### 2.2 74HC595 → 1N5817 diodes → DIP switch → J5 (relays)

Insert **1N5817** (anode to 74HC595, cathode to relay node) between each 74HC595 output and the DIP high pin. DIP low pin → GND.

| Relay CH | 74HC595 U2 out | J5 pin | DIP high pin (→ relay node) | DIP low pin (→ GND) |
|----------|----------------|--------|-----------------------------|---------------------|
| CH1 | U2-15 | J5-2 | SW1-13 | SW1-1 |
| CH2 | U2-1 | J5-3 | SW1-14 | SW1-2 |
| CH3 | U2-2 | J5-4 | SW1-15 | SW1-3 |
| CH4 | U2-3 | J5-5 | SW1-16 | SW1-4 |
| CH5 | U2-4 | J5-6 | SW1-17 | SW1-5 |
| CH6 | U2-5 | J5-7 | SW1-18 | SW1-6 |
| CH7 | U2-6 | J5-8 | SW1-19 | SW1-7 |
| CH8 | U2-7 | J5-9 | SW1-20 | SW1-8 |

- 74HC595 control legs: SER (U2-14) → GPIO0; SRCLK (U2-11) → GPIO23; RCLK (U2-12) → GPIO15; OE (U2-13) → GND; SRCLR (U2-10) → 3.3V via 10kΩ; VCC (U2-16) → 3.3V; GND (U2-8) → GND.
- J5-P10 → +5V (relay module VCC). J5-P1 → GND.

### 2.3 Power rails

Wire shared nets: `+5V`, `3.3V`, `GND`.

| From | To | Net |
|------|----|-----|
| +5V bus | J5-P10, J2_PCA9685-Px, ZMPT101B, PCA9685 VCC | `+5V` |
| GND | U1 GND, U2-8, J5-P1, J2_PCA9685, all M1–M10 | `GND` |
| 3.3V | U2-16 (VCC), SRCLR control, I2C pull-ups (verify), 1N5817 diode nodes | `3.3V` |
### 2.4 PWM — 10 ESP32 LEDC channels

All 10 LEDC channels at 10 kHz. No external pull-down (D4814 modules fail-safe OFF). CCT pairs on adjacent pins.

| # | Function | ESP32 GPIO | Notes |
|---|----------|-----------|-------|
| 1 | CEILING_WARM | GPIO18 | CCT pair (high-speed CH0) |
| 2 | CEILING_COLD | GPIO19 | CCT pair (high-speed CH1) |
| 3 | DESK1_WARM | GPIO25 | CCT pair (CH2) |
| 4 | DESK1_COLD | GPIO26 | CCT pair (CH3) |
| 5 | DESK2_WARM | GPIO4 | CCT pair (CH4, strapping) |
| 6 | DESK2_COLD | GPIO5 | CCT pair (CH5, strapping) |
| 7 | PHOTO_SPOT_LIGHT | GPIO12 | single |
| 8 | WALL_BOUNCE_LIGHT | GPIO13 | single |
| 9 | WASHROOM_LIGHT | GPIO33 | single |
| 10 | TERRACE_LIGHT | GPIO27 | single |

### 2.5 PWM — 14 PCA9685 channels (32 + 2 backup)

PCA9685 @0x40, driven over I2C (GPIO21 SDA / GPIO22 SCL). OE is strapped LOW in the module (outputs always enabled). All at 1 kHz. Backups (CH14/CH15) are **named, not unassigned** — keep them assigned so future devices map without restructuring.

| PCA9685 CH | Function |
|------------|----------|
| CH0 | MONITOR1_BACKLIGHT |
| CH1 | DESK_LIGHT1 |
| CH2 | TABLE_LIGHT |
| CH3 | SHELF_LIGHT1 |
| CH4 | MONITOR2_BACKLIGHT |
| CH5 | DESK_LIGHT2 |
| CH6 | SHELF_LIGHT2 |
| CH7 | CUPBOARD_BOUNCE_LIGHT |
| CH8 | CUPBOARD_SPOT_LIGHT |
| CH9 | DOOR_SPOT_LIGHT |
| CH10 | MONITOR3_BACKLIGHT |
| CH11 | MONITOR4_BACKLIGHT |
| CH12 | EXHAUST_FANS (shared) |
| CH13 | TERRACE_AMBIENT_LIGHT |
| CH14 | BACKUP_01 (reserved) |
| CH15 | BACKUP_02 (reserved) |

Wire each PCA9685 channel output leg to the corresponding load. Do not leave CH14/CH15 floating unassigned — terminate the net with a BACKUP_01 / BACKUP_02 label.

---

## 3. Connection labels to add (KiCad "Connection/GMD" fields)

Label these nets so connectivity is legible:
`+5V`, `3.3V`, `GND`, `I2C_SDA`, `I2C_SCL`, `SER`, `SRCLK`, `RCLK`, `ZMCT103C_CURRENT`, `ZMPT101B_VOLTAGE`, `DOOR_SENSOR`, `IR_RX`, `RELAY_30A`, and the 24 PWM nets: `CEILING_WARM`, `CEILING_COLD`, `DESK1_WARM`, `DESK1_COLD`, `DESK2_WARM`, `DESK2_COLD`, `PHOTO_SPOT_LIGHT`, `WALL_BOUNCE_LIGHT`, `WASHROOM_LIGHT`, `TERRACE_LIGHT`, `MONITOR1_BACKLIGHT`, `DESK_LIGHT1`, `TABLE_LIGHT`, `SHELF_LIGHT1`, `MONITOR2_BACKLIGHT`, `DESK_LIGHT2`, `SHELF_LIGHT2`, `CUPBOARD_BOUNCE_LIGHT`, `CUPBOARD_SPOT_LIGHT`, `DOOR_SPOT_LIGHT`, `MONITOR3_BACKLIGHT`, `MONITOR4_BACKLIGHT`, `EXHAUST_FANS`, `TERRACE_AMBIENT_LIGHT`, plus `BACKUP_01`, `BACKUP_02`.

---

## 4. Validation (after wiring)

Once placed and wired in KiCad, I will run:
1. `generate_netlist` → export `/tmp` netlist
2. `get_netlist_nets` + `trace_netlist_connection` → check all 13 signal nets + rails present
3. `run_erc` → Electrical Rules Check (unconnected pins, conflicts)
4. After PCB layout: `run_drc` + `export_erc_report` / `export_drc_report`

Record results in `docs/release-evidence.md` per Gate 4.

---

*Source of truth cross-refs: docs/pin-budget.md §2–§6, PCB_DESIGN_GUIDE.md §4A.*