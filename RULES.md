# Auto Mate Rules

- `SPECIFICATIONS.md` is the source of truth.
- Work in small phases and link requirement/test IDs.
- Do not guess GPIOs, electrical ratings, component choices, load behavior, or safety limits.
- Keep secrets outside Git and never commit personal/customer data.
- Use modular ESPHome configuration and product-owned assembly files.
- Use KiCad 10 and `kicad-cli` for validation; AI-generated hardware requires human review.
- Preserve explicit behavior for restart, reconnect, unavailable integrations, and invalid commands.
- Update documentation, traceability, and release evidence whenever behavior changes.
- Use absolute local paths in operational commands and setup documentation.
