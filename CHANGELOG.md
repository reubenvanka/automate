# AutoMate IOT Project - CHANGELOG

## [2026-09-16] - Version 1.0.0 (firmware-v0.1.2 based)
### Initial working version restored from firmware-v0.1.2 tag
- Restored all packages from firmware-v0.1.2 tag (core.yaml, pwm.yaml, pca9685.yaml, relays.yaml)
- Base configuration with:
  - 9 relays via 74HC595 + 1 direct 30A relay
  - 10 LEDC PWM outputs (2 CCT ceiling, 2 CCT desk1, 2 CCT desk2, 4 monochromatic)
  - 14 PCA9685 monochromatic lights
  - Template switches for all relays
  - Binary sensor automations for:
    - non_ceiling_pwm_active (12V driver)
    - ceiling_pwm_active (ceiling driver)
    - desks_active (voltage stabilizer)
  - Web server with auth, captive portal, OTA
  - NVS preferences with 30s write interval
  - Basic boot sequence (no state restoration)

---

## [2026-09-16] - Version 1.0.1
### Added state persistence and immediate NVS writes
- Changed `flash_write_interval: 30s` → `0s` for immediate NVS writes
- Added globals for relay state persistence (fan, ceiling_driver, 12v_driver, stabilizer, desk1-3, 16a, 30a)
- Added globals for light state persistence (ceiling, desk1, desk2, photo_spot, wall_bounce, terrace, washroom)
- Added boot state restoration logic in core.yaml
- Added state saving on light/relay changes
- Fixed template switches to read from physical relays (`.state`)
- Changed template switch actions to use `switch.turn_on/off` instead of `output.turn_on/off`
- Set all physical relays as `internal: true` (hidden from HA)
- Added `on_turn_on/off` handlers to all lights to save state immediately
- Fixed non_ceiling_pwm_active to include all non-ceiling lights
- Added desk1/desk2 prev_state globals to pwm.yaml

---

## [2026-09-16] - Version 1.0.2
### Fixed 12V driver automation and added PCA9685 light state persistence
- Added `desk1_prev_state`, `desk2_prev_state` globals to pwm.yaml (with restore_value: yes)
- Added `desk3_prev_state` global to relays.yaml (desk3 relay exists but no CCT light)
- Fixed `non_ceiling_pwm_active` binary sensor to include ALL non-ceiling lights:
  - Desk CCT lights (desk1, desk2)
  - LEDC monochromatic (photo_spot, wall_bounce, terrace, washroom)
  - PCA9685 monochromatic (monitor backlights, desk lights, table, shelf lights, cupboard, door, exhaust, terrace_ambient)
- Added `on_turn_on`/`on_turn_off` handlers to ALL PCA9685 lights to control 12V driver
- Added `on_state` handlers to ALL PCA9685 lights to save state immediately
- Added globals for all PCA9685 lights (state + brightness)
- Fixed output IDs in PCA9685 lights (added missing `_light` suffixes)
- Fixed desk3_prev_state global in relays.yaml (needed for desk3 relay)

---

## [2026-09-16] - Version 1.0.3
### Fixed 12V driver last state restoration
- Added `desk1_prev_state`, `desk2_prev_state` globals with `restore_value: yes`
- Added `desk3_prev_state` global to relays.yaml with `restore_value: yes`
- Removed desk1/desk2 prev_state from relays.yaml (only in pwm.yaml)
- Fixed relay state restoration in boot sequence
- Fixed immediate NVS writes (`flash_write_interval: 0s`)
- Ensured all state changes save immediately to NVS
- Fixed 12V driver automation to use `switch.turn_on/off` consistently
- Added state saving in `on_state` for all lights (real-time persistence)

### Files Modified:
- `packages/core.yaml` - Boot restoration, NVS settings, flash_write_interval
- `packages/relays.yaml` - Globals, internal relays, template switches, automations
- `packages/pwm.yaml` - Light globals, light handlers, desk state globals
- `packages/pca9685.yaml` - All 16 PCA9685 lights with on_turn_on/off and state saving

---

## Versioning Scheme
- **Major**: Breaking changes, hardware changes
- **Minor**: New features, significant fixes
- **Patch**: Bug fixes, small improvements

---

## Backup Location
All firmware binaries are stored in:
- `.esphome/build/automate/.pioenvs/automate/firmware.bin`
- `.esphome/build/automate/.pioenvs/automate/firmware.factory.bin`
- `.esphome/build/automate/.pioenvs/automate/firmware.ota.bin`

Git tags used for versioning:
- `firmware-v0.1.0` - Initial compilable assembly
- `firmware-v0.1.1` - GPIO fix
- `firmware-v0.1.2` - Health monitoring + activity LED + DHCP (working baseline)
- Current working directory state tracked via git commits