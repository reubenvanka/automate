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
