# AutoMate Requirement Traceability

| Requirement ID | Requirement | Design/reference | Test ID | Status | Evidence |
| --- | --- | --- | --- | --- | --- |
| AM-REQ-001 | Coordinated lighting and fan control. | `docs/architecture.md` | AM-TEST-001 | TODO | TODO |
| AM-REQ-002 | Brightness control where supported. | `docs/architecture.md` | AM-TEST-002 | TODO | TODO |
| AM-REQ-003 | Color-temperature control where supported. | `docs/architecture.md` | AM-TEST-003 | TODO | TODO |
| AM-REQ-004 | Environmental sensor data. | `docs/architecture.md` | AM-TEST-004 | TODO | TODO |
| AM-REQ-005 | Home Assistant integration. | `docs/architecture.md` | AM-TEST-005 | TODO | TODO |
| AM-REQ-006 | Authenticated local/custom web interface. | `docs/architecture.md` | AM-TEST-006 | TODO | TODO |
| AM-REQ-007 | Safe restart/reconnect/invalid-command/brownout and valid desired-state restoration. | `SPECIFICATIONS.md`, `docs/architecture.md` | AM-TEST-007 | TODO | TODO |
| AM-REQ-008 | Secret and privacy protection. | `RULES.md` | AM-TEST-008 | TODO | TODO |
| AM-REQ-009 | Human approval for hardware and release decisions. | `docs/development-gates.md` | AM-TEST-009 | TODO | TODO |
| AM-REQ-010 | Nine named relay functions. | `SPECIFICATIONS.md`, `docs/architecture.md` | AM-TEST-010 | TODO | TODO |
| AM-REQ-011 | 24 PWM outputs and three CCT fixtures: ten ESP32-direct LEDC channels (six CCT plus four single-channel) and fourteen PCA9685 single-channel outputs plus two PCA9685 backup channels; software/bit-banged PWM prohibited. | `SPECIFICATIONS.md`, `docs/architecture.md`, `docs/pin-budget.md` | AM-TEST-011 | TODO | TODO |
| AM-REQ-012 | Local ceiling-driver relay automation. | `SPECIFICATIONS.md`, `docs/architecture.md` | AM-TEST-012 | TODO | TODO |
| AM-REQ-013 | Local 12 V-driver relay automation. | `SPECIFICATIONS.md`, `docs/architecture.md` | AM-TEST-013 | TODO | TODO |
| AM-REQ-014 | Local stabilizer relay automation. | `SPECIFICATIONS.md`, `docs/architecture.md` | AM-TEST-014 | TODO | TODO |
| AM-REQ-015 | Maintenance override modes. | `SPECIFICATIONS.md`, `docs/architecture.md` | AM-TEST-015 | TODO | TODO |
| AM-REQ-016 | Local automation during client disconnection. | `docs/architecture.md` | AM-TEST-016 | TODO | TODO |
| AM-REQ-017 | Future acknowledged/reconciling ESP remote protocol; ESP-NOW deferred and transport TBD. | `SPECIFICATIONS.md`, `docs/architecture.md` | AM-TEST-017 | TODO | TODO |
| AM-REQ-018 | Use camera-conscious PWM frequencies with ten ESP32 hardware LEDC channels (six CCT plus four single-channel) and fourteen PCA9685 single-channel outputs; software/bit-banged PWM is prohibited. | `SPECIFICATIONS.md`, `docs/architecture.md`, `docs/pin-budget.md` | AM-TEST-018 | TODO | TODO |
| AM-REQ-019 | Monitor current and voltage using the reviewed ADC1-based ZMCT103C/ZMPT101B architecture and derive load, energy, and sensor-health values in software; no metering IC planned. | `SPECIFICATIONS.md`, `docs/architecture.md` | AM-TEST-019 | TODO | TODO |
| AM-REQ-020 | Deterministic boot, default-off, watchdog, safe-mode, OTA rollback, captive-portal recovery, network reconciliation, and boot recovery. | `SPECIFICATIONS.md`, `docs/architecture.md` | AM-TEST-020 | TODO | TODO |
| AM-REQ-021 | Provide bounded persistent, Home Assistant-editable scenes through native-API services; YAML-only scenes are insufficient. | `SPECIFICATIONS.md`, `docs/architecture.md`, `docs/release-evidence.md` | AM-TEST-021 | TODO | TODO |

Do not mark a requirement complete without linked evidence.
