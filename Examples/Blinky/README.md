# Blinky project

The **Blinky** project is a simple example that can be used to verify the
basic tool setup for the BBC micro:bit V2.

It is compliant to the Cortex Microcontroller Software Interface Standard (CMSIS)
and runs bare-metal: the SysTick timer provides a 1 ms time base.

## Operation

- vioLED0 (top-left LED of the 5x5 matrix) blinks with 1 Hz.
- vioLED1 (top row, second LED) is on while button A is pressed.
- vioLED2 (top row, third LED) is on while button B is pressed.

### CMSIS-Driver Virtual I/O mapping

| CMSIS-Driver VIO      | Board component
|:----------------------|:--------------------------------------
| vioBUTTON0            | Button A (P0.14)
| vioBUTTON1            | Button B (P0.23)
| vioLED0               | LED matrix row 1, column 1
| vioLED1               | LED matrix row 1, column 2
| vioLED2               | LED matrix row 1, column 3

## Build

The project is in *csolution format* for the [CMSIS-Toolbox](https://open-cmsis-pack.github.io/cmsis-toolbox/)
and the VS Code [Arm CMSIS Solution](https://marketplace.visualstudio.com/items?itemName=Arm.cmsis-csolution) extension.

```sh
cbuild Blinky.csolution.yml --packs --update-rte --context Blinky.Debug+microbit-v2 --toolchain GCC
```

The GCC linker script `RTE/Device/nRF52833_xxAA/gcc_linker_script.ld.src` is the CMSIS-Toolbox template with
one addition: it keeps the `.isr_vector` section of the Nordic startup file at the start of Flash. The memory
regions come from `RTE/Device/nRF52833_xxAA/regions_microbit-v2.h` (512 KB Flash at `0x00000000`, 128 KB RAM at
`0x20000000`, 512 bytes stack, 3 KB heap).

## Run

- Program and debug with the on-board DAPLink probe through the Arm CMSIS Debugger extension
  (pyOCD, CMSIS-DAP, SWD; configured in the solution's `target-set`), or
- copy `out/Blinky/microbit-v2/Debug/Blinky.hex` to the `MICROBIT` USB mass-storage drive.

### First download on a factory-programmed micro:bit

The factory firmware (MakeCode / CODAL) contains a Nordic SoftDevice. Nordic's standard flash algorithm
`nrf52xxx.flm`, which the DFP selects, refuses to erase SoftDevice sectors, so the first pyOCD download fails with
`flash erase sector failure (address 0x00000000; result code 0x67)`. Program the board once by drag-and-drop of
`Blinky.hex` to the `MICROBIT` drive (DAPLink removes the SoftDevice); after that pyOCD download and debug work.
Alternatively a mass erase or Nordic's `nrf52xxx_sde.flm` algorithm removes the SoftDevice.

### Verified on hardware

micro:bit V2.2 (DAPLink 0257), GCC 14.3.1, CMSIS-Toolbox 2.15.0, pyOCD 0.45.1 via the Arm CMSIS Debugger:
download, breakpoints in `main` and `vioInit`, LED matrix pins and SysTick timing checked with the debugger.
