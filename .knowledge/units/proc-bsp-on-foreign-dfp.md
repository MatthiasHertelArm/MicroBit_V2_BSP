---
id: proc-bsp-on-foreign-dfp
type: procedure
title: "bsp-on-foreign-dfp"
summary: "Create a Board Support Pack for a board whose MCU vendor DFP predates the CMSIS-Toolbox: decide BSP-only versus DFP+BSP, reuse the vendor pack, fix what the toolbox needs in the example, validate with packchk and on hardware."
domain: pack
status: draft
applies_to:
  tool: [packchk, csolution, gen-pack]
triggers:
  - creating a BSP for a board with an old vendor DFP (schema < 1.7, assembler startup, no linker scripts)
source: MicroBit::MicroBit_V2_BSP work 2026-10-09 to 2026-10-13
license: Apache-2.0
---

## Steps

1. Read the vendor DFP: `<devices>` (Dname, memories, algorithms, `debugconfig`), the `Device:Startup`
   component and its compiler conditions, and whether linker scripts exist. If the device, SVD and flash algorithms
   are right, prefer a BSP that `<requires>` the DFP over an own DFP (less to maintain, one source of truth).
2. Board description: `vendor`/`name` without spaces or colons (used as `Vendor::Name` in csolution),
   `mountedDevice` with the exact Dname, `mountedPart` for sensors and the interface MCU, `debugProbe`
   (`CMSIS-DAP`, `swd`, clock), `feature` elements only from the schema enumeration (`Bluetooth` is a device
   feature, not a board feature; use `Other`), books with URLs or pack-local PDFs whose licence allows redistribution.
3. Conditions: a board condition (`Bvendor`/`Bname`), a startup condition that adds a device filter
   (`Dvendor`/`Dname="…*"`) and `Device:Startup`; derive component conditions from it. packchk M336 flags
   `Board Support` components without a device filter.
4. Components: `CMSIS Driver:VIO:Board` for the toolbox blinky convention; `Board Support:*` drivers with one
   header; `RTE_Components_h` defines per component; `category="doc"` entries for datasheet URLs.
5. Examples: csolution with `select-compiler`, `target-set` with `debugger: name: CMSIS-DAP@pyOCD`, cproject
   with the components; when the vendor startup is GCC assembler, ship the adapted toolbox linker script and a
   `regions_<board>.h` in `RTE/Device/<Dname>/` and reference both in `linker:`.
6. Local test build before packaging: `packs:` entry with `pack: Vendor::Name` plus `path: ../..`; restore the
   plain entry afterwards. Remove `compiler:` lines the extension appends and the generated `.vscode/`,
   `.cmsis/tools-environment.yml`, `out/`, `tmp/`, `*.cbuild-*.yml` files from the shipped tree.
7. Validate: `packchk <pdsc> -i <DFP pdsc> -i ARM.CMSIS.pdsc [-i ARM.CMSIS-Compiler.pdsc]` to 0 errors and 0
   warnings; `gen_pack.sh` (gen-pack library; macOS needs GNU coreutils); then the hardware procedure of the board.
8. Write down what the DFP lacks and how the BSP works around it (concept unit) and every tool error met on the
   way with its fix (troubleshooting units).
