# Auto Mate KiCad Workflow

1. Create a KiCad 10 project skeleton after hardware architecture is approved.
2. Add approved symbols and candidate connections.
3. Export and inspect the netlist.
4. Establish a candidate PCB outline and initial placement.
5. Run ERC and DRC with `kicad-cli`.
6. Record all violations and human review decisions.
7. Require approval before final connections, placement, routing, safety, or manufacturing release.

## Local tool paths

- KiCad app: `/Applications/KiCad/KiCad.app`
- CLI: `/opt/homebrew/bin/kicad-cli`
- Bundled Python: `/Applications/KiCad/KiCad.app/Contents/Frameworks/Python.framework/Versions/3.9/bin/python3`

## Compatibility note

The existing Seeed Studio MCP server uses modern Python but KiCad's bundled `pcbnew` uses Python 3.9. Text parsing and CLI validation may work without `pcbnew`; API-backed editing requires a verified bridge or compatible server.
