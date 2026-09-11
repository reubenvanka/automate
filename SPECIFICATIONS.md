# Auto Mate Specification

**Status:** Draft foundation specification

**Source of truth:** This file. Implementation and documentation must not introduce behavior that conflicts with it.

## Product identity

- Product name: **Auto Mate**
- Repository slug: `auto-mate`
- Product category: Smart-room controller
- Current phase: Planning/foundation

## Confirmed product intent

Auto Mate is intended to coordinate a room's lighting and fan/environmental controls. The current confirmed scope includes:

1. Control lighting and fan outputs.
2. Adjust brightness where supported.
3. Adjust color temperature where supported.
4. Read environmental sensors.
5. Integrate with Home Assistant.
6. Provide a local/custom web interface.
7. Preserve safe, understandable behavior during reconnects, restarts, and unavailable integrations.

These are product goals, not approved electrical or GPIO decisions.

## Requirements

| ID | Requirement | Acceptance evidence |
| --- | --- | --- |
| AM-REQ-001 | Provide coordinated control of lighting and fan outputs. | Approved control-state model and integration/manual tests. |
| AM-REQ-002 | Support brightness control where the selected hardware and load support it. | Hardware capability documented and brightness tests pass. |
| AM-REQ-003 | Support color-temperature control where the selected lighting hardware supports it. | Hardware capability documented and color-temperature tests pass. |
| AM-REQ-004 | Read and expose environmental sensor data. | Approved sensor model, calibration approach, and data tests. |
| AM-REQ-005 | Integrate with Home Assistant using a documented, secret-safe interface. | Integration test or approved manual evidence; no credentials in Git. |
| AM-REQ-006 | Provide a local/custom web interface for approved control and status functions. | UI/API review and local integration tests. |
| AM-REQ-007 | Define safe behavior for restart, reconnect, unavailable integrations, and invalid commands. | Failure-mode tests and human UX/safety review. |
| AM-REQ-008 | Keep all secrets, credentials, personal/customer data, and private commercial information outside Git. | Secret scan and repository-history review. |
| AM-REQ-009 | Require human approval for GPIO, electrical, KiCad, safety, and release decisions. | Signed/recorded review in `docs/release-evidence.md`. |

## Hardware and electrical TODOs

The following must be resolved before hardware-dependent implementation:

- Controller/MCU model and revision: **TODO**.
- Exact GPIO and peripheral mapping: **TODO**; do not infer from a development board pin label.
- Power input, regulators, current budget, protection, and thermal limits: **TODO**.
- Lighting and fan load topology, switching devices, ratings, isolation, and fail-safe state: **TODO**.
- Sensor models, interfaces, ranges, accuracy, placement, and calibration: **TODO**.
- Indicator LEDs, display/OLED interface, buttons, enclosures, and environmental rating: **TODO**.
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

## Non-goals

- Manufacturing-ready AI-generated schematics or PCBs without human review.
- Guessing GPIO assignments, electrical ratings, or safety limits.
- Committing secrets or customer data.
- Treating a candidate web interface as secure before threat and authorization review.

## Change control

Behavioral changes require a specification update, linked requirement/test IDs, documentation updates, and review of affected tests and release evidence. Hardware changes require an approved mapping and electrical/safety review.
