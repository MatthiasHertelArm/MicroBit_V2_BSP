---
id: rule-drag-and-drop-when-pyocd-erase-fails
type: rule
title: "On DAPLink boards, use the mass-storage programmer once when the vendor flash algorithm refuses to erase"
summary: When pyOCD's sector erase fails with an algorithm result code on a board that also offers DAPLink drag-and-drop programming, copy the .hex to the drive once instead of changing the debugger configuration. It removes vendor bootloaders or SoftDevices and leaves the flash in a state the standard algorithm accepts.
domain: debug
status: draft
applies_to:
  tool: [pyocd, daplink]
  board: [microbit-v2, microbit-v1, nRF52-DK, FRDM-*]
source: own observation 2026-10-13
license: Apache-2.0
---

## Rule

Do not switch flash algorithms, hand-edit the cbuild-run file or run `pyocd erase --chip` repeatedly when
`flash erase sector failure (...; result code 0x..)` appears on a DAPLink board. Copy the application `.hex` to
the DAPLink drive once (unmount to flush on macOS; wait for `Remount count` to increment and check for
`FAIL.TXT`), then continue with `cmsis_action load_and_debug`.

## Why

The result code comes from the vendor algorithm, which protects regions it considers special (Nordic SoftDevice,
NXP bootloader). DAPLink's own programmer does not apply that policy. One drag-and-drop puts the flash into a
state the algorithm accepts, and no project configuration changes are left behind.

## How to apply

Needs the `.hex` output type in the cproject (`output: type: [elf, hex]`). The DAPLink mass-storage drive is a
host-side file copy and does not count as driving the probe from a shell.
