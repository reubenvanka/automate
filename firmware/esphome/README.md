# AutoMate ESPHome Firmware

This directory holds the product-owned ESPHome assembly/configuration and focused modules.

## Structure

```
firmware/esphome/
├── configuration.yaml      # Main assembly — imports all packages
├── secrets.yaml.example    # Copy to secrets.yaml and fill in (never commit)
└── packages/               # Focused modules (merged by configuration.yaml)
    ├── core.yaml           # esphome, wifi, api, ota, logger, web_server, time, boot
    ├── pwm.yaml            # 10 ESP32 LEDC channels (6 CCT + 4 single) + lights
    ├── pca9685.yaml        # 1x PCA9685: CH0–13 assigned + BACKUP_01/02 (CH14/15)
    ├── relays.yaml         # 74HC595 (8 relays) + 30A direct relay (GPIO32) + switches
    ├── sensors.yaml        # AHT20, BMP280, BH1750, door (MC-38), IR (VS1838B)
    ├── energy-sensors.yaml # ZMCT103C current + ZMPT101B voltage (ADC1)
    └── state_machine.yaml  # Scenes, maintenance modes, safe restoration (custom component)
```

## Working with the project

```bash
# 1. Create secrets.yaml from the template
cp secrets.yaml.example secrets.yaml   # then fill in real values

# 2. Compile the assembly (merges all packages)
esphome compile configuration.yaml

# 3. Flash / run
esphome run configuration.yaml
```

## Notes

- The assembly **passes `esphome config`** (compiles at config stage). Only the 3 strapping-pin warnings (GPIO5/12/15 — accepted, board has no external pull) remain.
- Two ESPHome-constraint GPIO changes from the original budget: **74HC595 latch (RCLK) = GPIO15** (GPIO24 is not exposed in the ESP32 IO mux) and **door sensor = GPIO14** (GPIO34–39 do not support `INPUT_PULLUP`).
- **`state_machine.yaml` is NOT included** in the assembly: it uses `custom_component`, which ESPHome 2026 removed. Scene persistence/restore API services are currently stubbed logs and must be ported to an external component.
- Hardware-dependent configuration was only added after the GPIO/PWM transport was approved in `SPECIFICATIONS.md` and `docs/pin-budget.md`.
