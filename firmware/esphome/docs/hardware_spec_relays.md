# AutoMate Relay Hardware Specification
## 74HC595 + ULN2803 Darlington Driver + Manual Override

**Date:** 2026-09-15  
**Hardware revision:** V2.1 (ULN2803 added)  

## Wiring Diagram

```
ESP32 GPIO0 ──────┬──► 74HC595 SER (Data)
                  │
ESP32 GPIO23 ──────┼──► 74HC595 SRCLK (Clock)
                  │
ESP32 GPIO15 ──────┼──► 74HC595 RCLK (Latch)
                  │
                  │
                  ▼
           ┌─────────────┐
           │ 74HC595     │
           │ Q0-Q7 →     │
           │ (8 outputs) │
           └──────┬──────┘
                  │
         74HC595 Q0 ─┼──► ULN2803 In1 (pin 1)
         74HC595 Q1 ─┼──► ULN2803 In2 (pin 2)
               ...          │
         74HC595 Q7 ─┼──► ULN2803 In8 (pin 8)
                  │
         ULN2803 Out1 (pin 18) ─► Relay IN1
         ULN2803 Out2 (pin 17) ─► Relay IN2
               ...          │
         ULN2803 Out8 (pin 11) ─► Relay IN8
                  │
         ULN2803 Pin 9 (GND) ─► Common GND
         ULN2803 Pin 10 (COM) ─► Relay VCC (5V/12V)
                  │
         Manual Switch SW0 ─┼──► ULN2803 Out1 (pin 18) via 0Ω (wired-OR to GND)
         Manual Switch SW1 ─┼──► ULN2803 Out2 (pin 17) via 0Ω (wired-OR to GND)
               ...                ...
         Manual Switch SW7 ─┼──► ULN2803 Out8 (pin 11) via 0Ω (wired-OR to GND)
                  │
         Dip Switch Common ─► GND (other side of all switches)
```

## Logic

### Active-LOW Relay Modules (Relay ON = input LOW / output LOW)

| 74HC595 Bit | ULN2803 Input | ULN2803 Output | Relay |
|-------------|---------------|----------------|-------|
| `1` (HIGH)  | HIGH          | LOW            | **ON** |
| `0` (LOW)   | LOW           | HIGH-Z         | **OFF** |

### Manual Override (Dip Switch Closed = GND)

| 74HC595 Bit | Manual Switch | ULN2803 Output | Relay |
|-------------|---------------|----------------|-------|
| Don't care  | **Closed**    | **LOW**        | **ON** |
| Don't care  | Open          | Depends on 74HC595 | Depends |

### Wired-OR Operation

Either ESP32 control OR manual switch can turn relay ON. Both must release for relay to turn OFF.

- ESP32 sets 74HC595 bit `1` → ULN2803 input `HIGH` → output `LOW` → **Relay ON**
- ESP32 sets 74HC595 bit `0` → ULN2803 input `LOW` → output `HIGH-Z` → **Relay can turn OFF**
- Manual switch closes → ULN2803 output pulled to `GND` → **Relay ON** (regardless of 74HC595 state)
- Both release → Relay returns to state determined by 74HC595

## Parts List (per 8-channel board)

| Qty | Part | Description |
|-----|------|-------------|
| 1 | 74HC595 | Shift register (3.3V tolerant inputs) |
| 1 | ULN2803 | 8-channel Darlington driver + built-in flyback diodes |
| 8 | BAT54/1N5819 | **NOT NEEDED** — ULN2803 eliminates input diodes |
| 8 | DIP Switches | Manual override (one per channel) |
| 8 | 10kΩ Resistors | Optional pull-ups if needed |
| 1 | 5V/12V PSU | For ULN2803 COM pin and relay VCC |
| 1 | Common GND | All ground connections |

**Total extra parts vs bare 74HC595:** +1 chip (ULN2803) + 8 dip switches

## ESPHome Configuration (relays.yaml)

Key changes from previous 74HC595-only config:

```yaml
# Previous (74HC595 only, weak 6mA drive):
sn74hc595:
  - id: sr_relays
    data_pin: GPIO0
    clock_pin: GPIO23
    latch_pin: GPIO15
    sr_count: 1

# New (74HC595 + ULN2803, 500mA drive):
sn74hc595:
  - id: sr_relays
    data_pin: GPIO0
    clock_pin: GPIO23
    latch_pin: GPIO15
    sr_count: 1

# Logic is inverted=True because:
# ESPHome turn_on → register bit=1 → 74HC595 Q=HIGH → ULN2803 in=HIGH → out=LOW → Relay ON
# ESPHome turn_off → register bit=0 → 74HC595 Q=LOW → ULN2803 in=LOW → out=HIGH-Z → Relay OFF
# `inverted: true` already present in previous config — no YAML change needed for logic
```

## Diagnostics

### Voltage Measurements (verify)

| Condition | Measure Qx (74HC595) to GND | Measure ULN2803 Outx to GND | Relay State |
|-----------|----------------------------|----------------------------|-------------|
| ALL OFF   | ~3.3V/5V | ~3.3V/5V (HIGH-Z) | All OFF |
| Q0 ON     | ~0V | ~0V | Q0 ON |
| Q0 OFF    | ~3.3V/5V | ~3.3V/5V (HIGH-Z) | Q0 OFF |
| Manual SW0 closed | Depends on 74HC595 | **~0V** | **FORCED ON** |

### Troubleshooting

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| Relays won't turn ON | 74HC595 not pulsing RCLK | Pulse LATCH pin after shiftOut |
| Relays weak/hunt | 74HC595 only 6mA → ULN2803 not driving | Verify ULN2803 pin 10 → Relay VCC |
| Relay turns ON then OFF | Floating OE/G pin | Tie OE/G (pin 13) to GND |
| Manual switch not working | Switch rating too low | Use dip switches rated 5V/10mA+ |
| Relays ON but manual override doesn't | Missing ULN2803 COM → VCC | Wire pin 10 to relay VCC |

## Safety

- ULN2803 COM (pin 10) **MUST** connect to relay VCC to enable built-in flyback diodes
- All GNDs must be common: ESP32, 74HC595, ULN2803, relay module, 5V PSU
- Manual switches at output stage — software cannot detect manual state (hardware-only override)
- Brownout/reset: ULN2803 outputs go HIGH-Z (relays de-energize) when ESP32 resets (OE/G tied to GND enables this)
- Brownout/reset: 74HC595 outputs go LOW (relays OFF) when ESP32 resets (LATCH pulsed low in setup)
- Verify relay module has optocoupler pull-up for wired-OR to work (standard modules do)