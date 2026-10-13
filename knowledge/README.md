# Knowledge units for the CMSIS Developer Assistant

Each file is one knowledge unit in the format of the CDA knowledge base (YAML front matter with `id`, `type`,
`title`, `summary`, `domain`, `status`, `applies_to`, optional `symptoms`/`symptom_regex` for troubleshooting
units, `source` and `license`, followed by the body). Types: `concept`, `procedure`, `troubleshooting`, `rule`.
They were written while creating and validating the `MicroBit::MicroBit_V2_BSP` pack on a micro:bit V2.2
(DAPLink 0257) with CMSIS-Toolbox 2.15.0, GCC 14.3.1 and pyOCD 0.45.1, and are meant to be imported into the
knowledge database.

| File | Type | Topic |
|:-----|:-----|:------|
| `ts-pyocd-microbit-softdevice-erase-0x67.md` | troubleshooting | pyOCD `flash erase sector failure ... result code 0x67` on a factory micro:bit |
| `ts-daplink-maintenance-mode.md` | troubleshooting | Board enumerates as `MAINTENANCE`, no CMSIS-DAP |
| `ts-csolution-local-pack-path-schema.md` | troubleshooting | `schema check failed` for a local pack without `pack:` |
| `ts-csolution-compiler-undefined-no-toolchain.md` | troubleshooting | `compiler undefined` / `no compiler registered` in the VS Code build |
| `concept-nordic-dfp-cmsis-toolbox.md` | concept | What the Nordic nRF_DeviceFamilyPack 8.44.1 gives the toolbox and what it lacks |
| `concept-microbit-v2-hardware.md` | concept | micro:bit V2 parts, pin map, electrical notes from the schematic |
| `concept-microbit-v2-bsp-components.md` | concept | Components and examples of MicroBit::MicroBit_V2_BSP |
| `concept-lsm303agr-i2c.md` | concept | LSM303AGR registers, addresses, scaling, as used and verified |
| `concept-nrf52-peripheral-recipes.md` | concept | Minimal register sequences for UARTE, TWIM, SAADC, PWM, TIMER, TEMP on nRF52833 |
| `proc-microbit-v2-bring-up.md` | procedure | Build, download and verify a csolution on the micro:bit V2 |
| `proc-bsp-on-foreign-dfp.md` | procedure | Writing a BSP pack on top of a vendor DFP that predates the toolbox |
| `rule-drag-and-drop-when-pyocd-erase-fails.md` | rule | Use the mass-storage programmer before fighting a vendor flash algorithm |
