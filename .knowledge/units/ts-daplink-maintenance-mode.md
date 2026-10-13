---
id: ts-daplink-maintenance-mode
type: troubleshooting
title: "micro:bit shows a MAINTENANCE drive and no CMSIS-DAP probe"
summary: "The DAPLink interface MCU is in bootloader mode (reset held while plugging in): no MICROBIT drive, no CMSIS-DAP probe, no serial port. Unplug and replug the board without touching reset."
domain: debug
status: draft
tool: daplink
severity: error
symptoms:
  - "Daplink Mode: Bootloader"
  - "No connected debug probes"
  - "Waiting for a debug probe"
applies_to:
  tool: [pyocd, daplink, cmsis-debugger]
  board: [microbit-v2]
source: own observation 2026-10-13 on micro:bit V2.2 (DAPLink 0257)
license: Apache-2.0
confidence: high
---

## Symptom

`/Volumes/MAINTENANCE` (macOS) or a `MAINTENANCE` drive is mounted instead of `MICROBIT`. Its `DETAILS.TXT`
says `Daplink Mode: Bootloader`. `ioreg`/USB listing shows no "BBC micro:bit CMSIS-DAP" device, `serial_list_ports`
shows no `usbmodem` port for the board, and every flash or debug attempt reports that no probe is present.

## Cause

DAPLink enters its bootloader when the reset button is held while the USB cable is plugged in. In this mode only
the interface firmware can be updated; the target MCU is not reachable.

## Fix

Unplug the USB cable and plug it in again without touching the reset button. The board re-enumerates as
"BBC micro:bit CMSIS-DAP" with a `MICROBIT` drive (`Daplink Mode: Interface`) and a new serial port. The serial
device name may change (for example `/dev/tty.usbmodem1102` became `/dev/tty.usbmodem11102`): call
`serial_list_ports` again before `serial_open`.
