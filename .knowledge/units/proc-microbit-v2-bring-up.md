---
id: proc-microbit-v2-bring-up
type: procedure
title: "microbit-v2-bring-up"
summary: Build a csolution for the BBC micro:bit V2 with the CMSIS Solution extension, download it with pyOCD over the on-board DAPLink probe, and verify it with the CMSIS Developer Assistant tools (breakpoints, peripheral registers, serial output).
domain: project
status: draft
applies_to:
  board: [microbit-v2]
  tool: [pyocd, cmsis-debugger]
triggers:
  - first download to a micro:bit V2
  - validating a pack or example on the micro:bit V2
preconditions:
  - a csolution with board MicroBit::microbit-v2 and the Nordic DFP, GCC linker script with .isr_vector (see concept-nordic-dfp-cmsis-toolbox)
  - vcpkg-configuration.json in the VS Code workspace root (see ts-csolution-compiler-undefined-no-toolchain)
stop_conditions:
  - "MAINTENANCE drive instead of MICROBIT: ask the user to replug the board (ts-daplink-maintenance-mode)"
  - "flash erase sector failure 0x67: program once by drag-and-drop (ts-pyocd-microbit-softdevice-erase-0x67)"
references: [concept-nordic-dfp-cmsis-toolbox, ts-csolution-compiler-undefined-no-toolchain, ts-daplink-maintenance-mode,
  ts-pyocd-microbit-softdevice-erase-0x67]
source: own work 2026-10-09 to 2026-10-13
license: Apache-2.0
---

## Steps

1. Confirm the probe: the USB device "BBC micro:bit CMSIS-DAP" (VID 0x0D28, PID 0x0204), a `MICROBIT` drive with
   `DETAILS.TXT` saying `Daplink Mode: Interface`, and a `usbmodem` port in `serial_list_ports`. The port name
   changes after a replug.
2. Pin the VS Code window (`select_debug_window`), `cmsis_action open_solution` with the csolution path, then
   `cmsis_action build`. Expect `compiler: GCC` to be appended to the csolution by the extension.
3. Check the image with the build-artefact tools: `lookup_symbol __isr_vector` must be at 0x00000000;
   `get_memory_usage` shows IROM1/IRAM1 usage; the cbuild-run has `programming:` entries for `nrf52xxx.flm` and
   `nrf52xxx_uicr.flm` and `debugger: CMSIS-DAP@pyOCD`.
4. `serial_open` the board's port at 115200 before the download so no output is lost.
5. `add_breakpoint` at the first line of `main` and in the first board function, then
   `cmsis_action load_and_debug`. On a factory board the first load fails with result code 0x67: copy the `.hex`
   to the `MICROBIT` drive, `diskutil unmountDisk` it, wait for the remount, then repeat.
6. `continue_execution` to each breakpoint, `get_call_stack`, `evaluate_expression` of driver state (for the
   BSP: `mb_fb`, `mb_row`, `NRF_P0->OUT`, `NRF_P0->DIR`), `read_peripheral_register` of the configured peripherals
   (`UARTE0.BAUDRATE` 0x01D60000, `TWIM0.FREQUENCY` 0x06400000, `TIMER1.CC[0]` 1000).
7. `clear_all_breakpoints`, `continue_execution`; `serial_read` the status output and `serial_write` single-key
   commands; `pause_execution` once more to sample `NRF_P0->OUT` (rows cycling) and `mb_ticks`.
8. Ask the user for the interactive checks (buttons, logo touch, sound, LED pattern); leave the target running,
   `serial_close` when done.
