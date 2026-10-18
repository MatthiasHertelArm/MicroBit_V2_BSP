# Knowledge of MicroBit::MicroBit_V2_BSP for the CMSIS Developer Assistant

`units/` holds the knowledge units of this pack, one Markdown file each: YAML frontmatter (`id`, `type`, `title`,
`summary`, `status`, `applies_to`, optional `symptoms`/`symptom_regex` for troubleshooting units, `source` and
`license`), followed by the text. The format is described in
[cmsis-developer-assistant-tools](https://github.com/MatthiasHertelArm/cmsis-developer-assistant-tools/blob/main/docs/format.md).
The units were written while creating and validating the pack on a micro:bit V2.2 (DAPLink 0257) with
CMSIS-Toolbox 2.15.0, GCC 14.3.1 and pyOCD 0.45.1.

`gen_pack.sh` builds them into `knowledge.db`, the index the assistant's knowledge tools read, and ships both:

```sh
pip install cmsis-developer-assistant-tools
cmsis-knowledge build --strict --fetch .
```

The assistant offers these units only in projects that use this pack.

| File | Type | Topic |
|:-----|:-----|:------|
| `units/ts-pyocd-microbit-softdevice-erase-0x67.md` | troubleshooting | pyOCD `flash erase sector failure ... result code 0x67` on a factory micro:bit |
| `units/ts-daplink-maintenance-mode.md` | troubleshooting | Board enumerates as `MAINTENANCE`, no CMSIS-DAP |
| `units/ts-csolution-local-pack-path-schema.md` | troubleshooting | `schema check failed` for a local pack without `pack:` |
| `units/ts-csolution-compiler-undefined-no-toolchain.md` | troubleshooting | `compiler undefined` / `no compiler registered` in the VS Code build |
| `units/concept-nordic-dfp-cmsis-toolbox.md` | concept | What the Nordic nRF_DeviceFamilyPack 8.44.1 gives the toolbox and what it lacks |
| `units/concept-microbit-v2-hardware.md` | concept | micro:bit V2 parts, pin map, electrical notes from the schematic |
| `units/concept-microbit-v2-bsp-components.md` | concept | Components and examples of MicroBit::MicroBit_V2_BSP |
| `units/concept-lsm303agr-i2c.md` | concept | LSM303AGR registers, addresses, scaling, as used and verified |
| `units/concept-nrf52-peripheral-recipes.md` | concept | Minimal register sequences for UARTE, TWIM, SAADC, PWM, TIMER, TEMP on nRF52833 |
| `units/proc-microbit-v2-bring-up.md` | procedure | Build, download and verify a csolution on the micro:bit V2 |
| `units/proc-microbit-v2-simulator.md` | procedure | Download mbsim and debug a csolution in the simulator (Arm-FVP adapter) |
| `units/proc-bsp-on-foreign-dfp.md` | procedure | Writing a BSP pack on top of a vendor DFP that predates the toolbox |
| `units/rule-drag-and-drop-when-pyocd-erase-fails.md` | rule | Use the mass-storage programmer before fighting a vendor flash algorithm |
