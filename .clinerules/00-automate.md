# AutoMate rules

- Read `SPECIFICATIONS.md`, `RULES.md`, and the applicable gate document before editing.
- Treat unresolved hardware and UX items as blockers for hardware-dependent implementation.
- Do not assign GPIOs or electrical ratings without an approved hardware file or specification update.
- Do not edit product Markdown merely to record a proposal, assumption, or provisional interpretation. Ask a clear clarification question first and obtain explicit confirmation that the decision is fixed.
- After a decision is fixed, update only the affected source-of-truth and linked documentation in one focused change; do not make speculative or broad rewrites.
- Keep Home Assistant tokens, Wi-Fi credentials, and web secrets outside Git.
- Use focused ESPHome packages and a product-owned assembly/configuration file.
- Run relevant build, lint, test, ERC, and DRC checks before reporting completion.
- Require human approval for final connections, placement, routing, safety, and release.
- **Backup workflow**: Every time a feature is added and the firmware compiles successfully, commit all source files (YAML packages, documentation, KiCad schematics/PCBs, etc.) and push to the GitHub repository. Tag the commit with a version (e.g., `firmware-vX.Y.Z`) to record the release.
- **Build artifacts backup**: After a successful compile, also backup the generated firmware binaries (`.bin`, `.ota.bin`, `.factory.bin`, `.elf`, `.map`) as GitHub Release assets attached to the corresponding tag. Keep these artifacts outside the main repository history (do not commit large binaries to the main branch).
- **Versioning**: Use semantic versioning for firmware tags (`firmware-vMAJOR.MINOR.PATCH`). Increment:
  - `MAJOR` for breaking changes or new major features,
  - `MINOR` for backward‑compatible new features,
  - `PATCH` for bug‑fixes and small improvements.
- Update `docs/release-evidence.md` (or equivalent) with the build outcome and flash evidence before pushing.
- Only push to GitHub after obtaining explicit human approval for the release (e.g., via a confirmation step).
