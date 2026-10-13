---
id: ts-pyocd-microbit-softdevice-erase-0x67
type: troubleshooting
title: "pyOCD: flash erase sector failure (address 0x00000000; result code 0x67) on a factory-programmed micro:bit V2"
summary: Nordic's standard flash algorithm nrf52xxx.flm refuses to erase sectors that hold a SoftDevice; the factory micro:bit firmware contains one. A chip erase through the same algorithm reports success but leaves the SoftDevice sectors, so the next sector erase fails again. Program the board once by drag-and-drop to the MICROBIT drive, or use a mass erase or Nordic's nrf52xxx_sde.flm.
domain: debug
status: draft
tool: pyocd
severity: error
symptoms:
  - "flash erase sector failure (address 0x00000000; result code 0x67)"
  - "'CMSIS Load' exited with code 1"
symptom_regex: "flash erase sector failure \\(address 0x[0-9A-Fa-f]+; result code 0x67\\)"
applies_to:
  tool: [pyocd, cbuild-run, cmsis-debugger]
  device: [nRF52833, nRF52840, nRF52832]
  board: [microbit-v2]
  pack: [NordicSemiconductor::nRF_DeviceFamilyPack]
source: own observation 2026-10-13 on micro:bit V2.2 (DAPLink 0257, pyOCD 0.45.1); pyocd/pyOCD issue 1262
license: Apache-2.0
confidence: high
---

## Symptom

`pyocd load --cbuild-run <solution>+<target>.cbuild-run.yml` (the CMSIS Load task of the Arm CMSIS Debugger) stops with

```
0000312 I Erasing... [loader]
0000338 C flash erase sector failure (address 0x00000000; result code 0x67) [__main__]
```

`cmsis_action load_and_debug` therefore reports that the pre-launch task `CMSIS Load` exited with code 1 and no
debug session starts. `cmsis_action erase` (`pyocd erase --chip --cbuild-run ...`) succeeds, but the next load fails
the same way.

## Cause

The cbuild-run file selects the flash algorithm the DFP marks as default. For the nRF52 family of
`NordicSemiconductor::nRF_DeviceFamilyPack` 8.44.1 that is `Flash/nrf52xxx.flm`. The algorithm detects a Nordic
SoftDevice in flash (the MakeCode/CODAL factory firmware of the micro:bit contains one) and refuses to erase its
sectors; the result code 0x67 is the algorithm's own error, not a probe error. Its chip erase leaves the SoftDevice
sectors in place, so erasing does not help. The pack also ships `Flash/nrf52xxx_sde.flm` ("SoftDevice erase",
`default="0"`), which the toolbox does not select.

## Fix

1. Copy the `.hex` of the application to the `MICROBIT` mass-storage drive once (on macOS unmount the drive
   afterwards: `diskutil unmountDisk` flushes the file; DAPLink remounts and `DETAILS.TXT` shows `Remount count: 1`).
   DAPLink erases the SoftDevice. From then on `pyocd load` with the standard algorithm works, including
   `cmsis_action load_and_debug`.
2. Alternatively perform a mass erase (`pyocd erase --mass -t nrf52833` with pyOCD's built-in target; not available
   through the cbuild-run target) or program once with `nrf52xxx_sde.flm`.

Do not try `pyocd erase --chip` first: it is reported as success and changes nothing for this failure.

## Evidence

Verified on 2026-10-13: load failed twice with 0x67 (before and after a successful chip erase), the drag-and-drop
of `Blinky.hex` removed the SoftDevice, the following `CMSIS Load` programmed in 2 s and the session halted at `main`.
