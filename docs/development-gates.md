# Auto Mate Development Gates

## Gate 1 — Planning

**Entry:** Product intent and initial requirements exist.

**Required evidence:**

- `SPECIFICATIONS.md` has Auto Mate requirement IDs.
- Hardware, UX, integration, security, and safety questions are listed.
- Product name and repository boundary are confirmed.

**Exit:** Planning review is recorded in `PROJECT_PLAN.md`.

## Gate 2 — Architecture

**Entry:** Planning gate passed.

**Required evidence:**

- Controller, power, load, sensor, communication, UX, and failure-mode architecture is approved or explicitly deferred.
- GPIO/peripheral mapping is approved before GPIO-dependent work.
- Security, safety, and test strategy are documented.

**Exit:** `docs/architecture.md` is reviewed and TODO ownership is recorded.

## Gate 3 — Firmware implementation

**Entry:** Architecture gate passed for the affected interface.

**Required evidence:**

- Focused changes with build and tests.
- No secrets or guessed hardware decisions.
- Documentation and traceability updated.

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

- Requirement/test traceability complete.
- Release notes, artifacts, recovery information, and rollback plan prepared.
- Human release approval recorded.

**Exit:** Tag and publish only after approval.
