# MicroBit_V2_BSP

The **BBC micro:bit V2 Board Support Pack (BSP)**:

- Describes the board (mounted Nordic nRF52833, on-board DAPLink debug probe, features) so that it can be
  selected as `MicroBit::microbit-v2` in csolution projects.
- Provides a CMSIS-Driver VIO implementation for the LED matrix and the buttons A and B.
- Provides Board Support drivers for every on-board peripheral: LED matrix, buttons, touch logo, speaker, microphone,
  LSM303AGR motion sensor, UART to the DAPLink interface (USB serial), die temperature, SysTick time base.
- Contains the examples Blinky and BoardTest in *csolution format* for the [CMSIS-Toolbox](https://open-cmsis-pack.github.io/cmsis-toolbox/)
  and the VS Code [Arm CMSIS Solution](https://marketplace.visualstudio.com/items?itemName=Arm.cmsis-csolution) extension.
- Ships the board schematic (CC BY-SA 4.0) and links the datasheets of the nRF52833, LSM303AGR and microphone.
- Contains knowledge units for the CMSIS Developer Assistant in `.knowledge/`.
- Requires the Device Family Pack `NordicSemiconductor::nRF_DeviceFamilyPack` (device header, startup and system
  files, SVD, flash algorithms for the nRF52833) and `ARM::CMSIS`.

## Content

| Directory / File       | Content
|:-----------------------|:---------------------------------------------------------------
| `Board`                | `vio_microbit_v2.c` CMSIS-Driver VIO; `microbit_*.c` Board Support drivers
| `Board/Include`        | `microbit_v2.h`: pin map and API of all Board Support components
| `Examples/Blinky`      | Blinky example (csolution), GCC linker script for the Nordic startup file
| `Examples/BoardTest`   | Exercises all peripherals with serial output and single-key commands
| `Documents`            | This overview, `MicroBit_V2.2.1_nRF52820_schematic.pdf`
| `.knowledge`           | Knowledge units (troubleshooting, concepts, procedures) for the CMSIS Developer Assistant

## Software components

| Component                           | Description
|:------------------------------------|:---------------------------------------------------------------
| `CMSIS Driver:VIO:Board`            | Virtual I/O: vioLED0..4 on LED matrix row 1, vioBUTTON0..1 on buttons A and B (not with `Board Support:LED Matrix`)
| `Board Support:GPIO`                | Pin map (`MB_PIN_*`), GPIO helpers, microsecond busy delay; required by the other Board Support components
| `Board Support:Time`                | SysTick millisecond time base: `microbit_time_init`, `microbit_time_ms`, `microbit_delay_ms`
| `Board Support:UART`                | UARTE0 to the DAPLink USB serial (TX P0.06, RX P1.08): `microbit_uart_init/putc/write/getc`
| `Board Support:I2C`                 | Internal I2C bus TWIM0 at 400 kHz: `microbit_i2c_write/read/write_read/read_reg/write_reg`
| `Board Support:Motion Sensor`       | LSM303AGR: `microbit_motion_init`, `microbit_accel_read` (mg), `microbit_mag_read` (mgauss)
| `Board Support:LED Matrix`          | 5x5 matrix with TIMER1 refresh: pixels, images, 5x5 font (`microbit_display_*`)
| `Board Support:Buttons`             | `microbit_button_pressed(MICROBIT_BUTTON_A/B)`
| `Board Support:Touch`               | Capacitive logo: `microbit_touch_init`, `microbit_touch_logo_pressed`, raw charge time
| `Board Support:Speaker`             | PWM0 tones: `microbit_speaker_tone(hz)`, `microbit_speaker_off`
| `Board Support:Microphone`          | SAADC on AIN3, RUN_MIC power: `microbit_mic_sample`, `microbit_mic_level`
| `Board Support:Temperature`         | nRF52833 die sensor: `microbit_temperature_read` in 0.25 C units

All Board Support components share the header `microbit_v2.h`. Standard I/O: use CMSIS-Compiler `STDOUT:Custom`
with `stdout_putchar` calling `microbit_uart_putc` (see `Examples/BoardTest/retarget_stdio.c`).

## Board

| Property       | Value
|:---------------|:---------------------------------------------------------------
| Board          | `MicroBit::microbit-v2` (revision V2.2; V2.0 boards differ only in the interface MCU)
| Device         | `Nordic Semiconductor::nRF52833_xxAA` (Cortex-M4F, 64 MHz, 512 KB Flash, 128 KB RAM)
| Debug probe    | On-board DAPLink (CMSIS-DAP, SWD) over USB micro-B
| Programming    | CMSIS-DAP or drag-and-drop of a `.hex` file to the `MICROBIT` drive

## Pin mapping

| Function            | nRF52833 pin
|:--------------------|:----------------
| LED matrix ROW1..5  | P0.21, P0.22, P0.15, P0.24, P0.19 (LED on: row high, column low)
| LED matrix COL1..5  | P0.28, P0.11, P0.31, P1.05, P0.30
| Button A / B        | P0.14 / P0.23 (active low, 10 kOhm pull-ups on the board)
| UART to DAPLink     | TX P0.06, RX P1.08 (USB serial on the host, 115200 8N1 in the examples)
| Internal I2C        | SCL P0.08, SDA P0.16, INT P0.25 (LSM303AGR 0x19/0x1E, interface MCU; 1 kOhm pull-ups)
| External I2C        | SCL P0.26 (P19), SDA P1.00 (P20); 4.7 kOhm pull-ups
| External SPI        | SCK P0.17 (P13), MISO P0.01 (P14), MOSI P0.13 (P15)
| Speaker             | P0.00 (MLT-8530 through a MOSFET)
| Microphone          | MIC_IN P0.05 / AIN3, RUN_MIC P0.20 (power, also lights the microphone LED)
| Touch logo          | P1.04 (10 MOhm pull-up on the board)
| Edge connector      | P0 P0.02, P1 P0.03, P2 P0.04, P8 P0.10, P9 P0.09, P12 P0.12, P16 P1.02; others shared as above

On-board parts (schematic V2.2.1): nRF52833-QIAA target, nRF52820-QDAA interface MCU (DAPLink; KL27Z on V2.0),
LSM303AGRTR, Knowles SPU0410LR5H-QB-7 analog microphone, MLT-8530 speaker, onsemi NCP114 3.3 V regulator.

Sources: `Documents/MicroBit_V2.2.1_nRF52820_schematic.pdf` (Micro:bit Educational Foundation, CC BY-SA 4.0),
[micro:bit V2 schematic and pin map](https://tech.microbit.org/hardware/schematic/),
[edge connector](https://tech.microbit.org/hardware/edgeconnector/).

## Notes on the Nordic DFP

The Nordic `nRF_DeviceFamilyPack` (8.44.1) predates the CMSIS-Toolbox. Its startup files are assembler files
(`gcc_startup_nrf52833.S`, `arm_startup_nrf52833.s`) and the pack ships no linker script that the toolbox can
use directly. The GCC startup places the vector table in a section named `.isr_vector`, which the toolbox linker
script template does not keep at the start of Flash. The Blinky example therefore carries a copy of the template
with `KEEP(*(.isr_vector))` added; copy `Examples/Blinky/RTE/Device/nRF52833_xxAA/` into your own project when
you build with GCC. The Arm Compiler 6 startup file uses the `RESET` area and works with the toolbox scatter template.

The DFP selects Nordic's standard flash algorithm `nrf52xxx.flm`. It refuses to erase sectors that hold a Nordic
SoftDevice, which the factory firmware of the micro:bit contains. The first download to a factory-programmed board
therefore fails under pyOCD; program the board once by drag-and-drop of a `.hex` file to the `MICROBIT` drive, then
pyOCD download and debug work (see the Blinky README).

## Simulator

[mbsim](https://github.com/MatthiasHertelArm/microbit-v2-sim) simulates this board (Cortex-M4F, the nRF52833
peripherals the drivers use, LED matrix, buttons, touch logo, LSM303AGR, microphone, speaker, UART); Blinky and
BoardTest run unmodified. It accepts the command line of the `Arm-FVP` debug adapter, so a target-type with
`debugger: name: Arm-FVP` and `model: <path to mbsim>` builds, runs and debugs in VS Code without a board.
The procedure is the knowledge unit `.knowledge/units/proc-microbit-v2-simulator.md`.

## Validation

Blinky and BoardTest were built with GCC 14.3.1 and CMSIS-Toolbox 2.15.0, downloaded with pyOCD 0.45.1
(CMSIS-DAP@pyOCD) and debugged on a micro:bit V2.2 (DAPLink 0257): vector table at Flash address 0, FICR reports
nRF52833 (512 KB Flash, 128 KB RAM), breakpoints in `main` and `vioInit` hit, LED matrix rows and columns driven
as documented, SysTick at 1 ms, UART at 115200 in both directions, LSM303AGR identified and read
(acceleration magnitude 1.02 g, magnetic field 0.5 to 0.75 gauss in the air), die temperature
about 21 C, buttons and touch logo (charge time 194 counts untouched, 4500 to 40000 touched), microphone bias 73 mV.
