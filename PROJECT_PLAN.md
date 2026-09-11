# Auto Mate Project Plan

## Phase 0 — Foundation

- Establish product repository, source-of-truth specification, AI context, rules, traceability, gates, and secret-safe ignore policy.
- Confirm product name and repository slug.

**Exit evidence:** Clean repository inspection and initial commit.

## Phase 1 — Requirements and architecture

- Select controller/MCU and hardware revision.
- Approve GPIO/peripheral mapping, power architecture, load topology, sensors, UX, and safety behavior.
- Define Home Assistant and local web contracts.
- Pass planning and architecture gates.

**Exit evidence:** Reviewed `SPECIFICATIONS.md`, `docs/architecture.md`, and closed/accepted TODOs.

## Phase 2 — Firmware foundation

- Add ESPHome product assembly/configuration and focused modules.
- Implement only approved hardware interfaces and control behavior.
- Validate YAML, build, unit tests, integration tests, and failure modes.

**Exit evidence:** Build/test results linked to Auto Mate requirement IDs.

## Phase 3 — Hardware candidate design

- Create KiCad project and candidate schematic.
- Export/inspect netlist and establish candidate PCB outline/placement.
- Run ERC/DRC and record human review decisions.

**Exit evidence:** KiCad reports, review notes, and approval status.

## Phase 4 — Integration and release

- Validate Home Assistant/local web behavior, reconnects, restarts, invalid commands, and recovery.
- Complete UX and safety review.
- Assemble release evidence and obtain human release approval.

**Exit evidence:** Release checklist, tagged commit, and approved artifacts.

## Current status

Phase 0 foundation is initialized, and the MCP/KiCad foundation workflow has been validated. Hardware-dependent implementation remains blocked on approved controller, GPIO, electrical, UX, and safety decisions.
