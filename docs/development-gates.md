# AutoMate Development Gates

## Gate 1 — Planning

**Entry:** Product intent and initial requirements exist.

**Required evidence:**

- `SPECIFICATIONS.md` has AutoMate requirement IDs.
- Hardware, UX, integration, security, and safety questions are listed.
- Product name and repository boundary are confirmed.

**Exit:** Planning review is recorded in `PROJECT_PLAN.md`.

## Gate 2 — Architecture

**Entry:** Planning gate passed.

**Required evidence:**

- Controller, power, load, sensor, communication, UX, and failure-mode architecture is approved or explicitly deferred, including ten ESP32-direct LEDC channels (six CCT plus four single-channel), fourteen PCA9685 single-channel outputs plus two PCA9685 backup channels, fixed `OE/G`-low/`SRCLR/MR`-high relay biases, software-derived diagnostics, persistent scenes, and ESP-NOW deferral.
- GPIO/peripheral mapping is approved before GPIO-dependent work; no GPIO number, PCA9685 address, or electrical implementation is inferred from the planning documents.
- Security, safety, recovery, scene persistence, and test strategy are documented.

**Exit:** `docs/architecture.md` is reviewed and TODO ownership is recorded.

## Gate 3 — Firmware implementation

**Entry:** Architecture gate passed for the affected interface.

**Required evidence:**

- Focused changes with build and tests, including hardware-LEDC/PCA9685 transport checks, scene persistence/validation, safe restoration, and software-derived diagnostics.
- No secrets, guessed hardware decisions, or software/bit-banged PWM fallback.
- Documentation, requirement traceability through `AM-REQ-021`, and release evidence updated.

**Exit:** Relevant build, lint, unit, integration, and failure-mode checks pass.

## Gate 4 — Hardware review

**Entry:** Candidate schematic/PCB exists.

**Required evidence:**

- Netlist inspection.
- ERC/DRC reports.
- Human approval for connections, placement, routing, safety, and manufacturing readiness.

**Exit:** Review decision is recorded in `docs/release-evidence.md`.

## Gate 5 — Release

**Entry:** Implementation and hardware review gates passed.

**Required evidence:**

- Requirement/test traceability complete through `AM-REQ-021`.
- Release notes, artifacts, recovery information, scene/restart evidence, software-derived monitoring evidence, and rollback plan prepared.
- Human release approval recorded.

**Exit:** Tag and publish only after approval.
