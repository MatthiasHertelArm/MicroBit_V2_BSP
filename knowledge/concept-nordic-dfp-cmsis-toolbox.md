---
id: concept-nordic-dfp-cmsis-toolbox
type: concept
title: "Using NordicSemiconductor::nRF_DeviceFamilyPack 8.44.1 with the CMSIS-Toolbox"
summary: The Nordic DFP (pdsc schema 1.3, 2021) loads in csolution 2.15 and provides device, SVD, flash algorithms and Device:Startup for nRF52833_xxAA, but its startup files are assembler files for ARMCC/GCC/IAR and it ships no toolbox linker script. The GCC startup keeps the vector table in .isr_vector, which the toolbox GCC linker template does not place at the start of Flash, so a BSP or project must carry an adapted linker script.
domain: pack
status: draft
applies_to:
  pack: [NordicSemiconductor::nRF_DeviceFamilyPack@8.44.1]
  device: [nRF52833_xxAA, nRF52840_xxAA, nRF52832_xxAA]
  tool: [csolution, cbuild, packchk]
source: pack content and own build on 2026-10-09 / 2026-10-13 with CMSIS-Toolbox 2.15.0, GCC 14.3.1
license: Apache-2.0
---

## What the pack provides

- `<family Dfamily="nRF52 Series" Dvendor="Nordic Semiconductor:54">` with `<debugconfig default="swd" swj="false"/>`
  and family-level algorithms `Flash/nrf52xxx.flm` (0x0, 2 MB, default) and `Flash/nrf52xxx_uicr.flm` (0x10001000,
  4 KB); `nrf52xxx_sde.flm` is non-default (SoftDevice erase).
- Device `nRF52833_xxAA`: Cortex-M4 r0p1, SP_FPU, MPU, 64 MHz, `compile header="Device/Include/nrf.h"
  define="NRF52833_XXAA"`, `debug svd="SVD/nrf52833.svd"`, memories `IROM1` 0x00000000/0x80000 (startup),
  `IRAM1` 0x20000000/0x20000, `IRAM2` 0x00800000/0x20000 (code-RAM alias, also `default="1"`; the toolbox maps it
  to `__RAM1` and leaves it unused).
- Component `Device:Startup` (condition on `Tcompiler`): `gcc_startup_nrf52833.S`, `arm_startup_nrf52833.s`,
  `iar_startup_nrf52833.s`, `system_nrf52.c` as config files, headers `system_nrf52833.h`, `system_nrf.h`.
  Component `Device:StartupConfig` adds `startup_config.h` (stack/heap sizes via `__STARTUP_CONFIG`).
- No linker scripts for the toolbox, no `regions_*.h`, no CMSIS-Driver, no VIO.

## Compatibility notes

- csolution 2.15 accepts the schema-1.3 pdsc without error. `memory id="IRAM1"` without `access` is mapped by name.
- GCC: the toolbox linker template `gcc_linker_script.ld.src` keeps `*(.vectors)`; the Nordic startup section is
  `.isr_vector`. Without `KEEP(*(.isr_vector))` the table is an orphan section placed after `.text`, not at 0.
  Fix: copy the template next to the generated `regions_<board>.h` in `RTE/Device/nRF52833_xxAA/`, add
  `KEEP(*(.isr_vector))` before `KEEP(*(.vectors))`, and reference both in the cproject `linker:` node.
- The Nordic GCC startup also defines `__StackTop`, `__StackLimit`, `__HeapBase`, `__HeapLimit` in its own
  `.stack`/`.heap` sections (no `aw` flags, so they stay non-allocated); the linker-script assignments win. It copies
  `.data` from `__etext` up to `__bss_start__` (one word past `__data_end__` at most) and relies on newlib `_start`
  to clear `.bss`. Verified: vector table at 0 (`0x20020000`, `Reset_Handler|1`), 2 KB blinky runs.
- AC6: `arm_startup_nrf52833.s` (armasm syntax, `-masm=auto` in cdefault) uses the `RESET` area, which the toolbox
  scatter template places first; it allocates its own 8 KB stack and heap in addition to `ARM_LIB_STACK`. Not built.
- `SystemInit` (system_nrf52.c) applies the nRF52 errata workarounds, enables the FPU when `__FPU_USED`, handles
  APPROTECT and sets `SystemCoreClock = 64000000`. `SystemCoreClockUpdate()` is a constant assignment.
- Flash download: see `ts-pyocd-microbit-softdevice-erase-0x67` for boards that carry a SoftDevice.
