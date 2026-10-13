---
id: concept-microbit-v2-bsp-components
type: concept
title: "MicroBit::MicroBit_V2_BSP: components, examples and how to use them"
summary: "Board Support Pack on top of the Nordic DFP and ARM::CMSIS: board MicroBit::microbit-v2, CMSIS-Driver VIO, and Board Support components (GPIO, Time, UART, I2C, Motion Sensor, LED Matrix, Buttons, Touch, Speaker, Microphone, Temperature) behind microbit_v2.h; examples Blinky and BoardTest."
domain: pack
status: draft
applies_to:
  pack: [MicroBit::MicroBit_V2_BSP@0.1.0]
  board: [microbit-v2]
source: pack source 2026-10-13
license: Apache-2.0
---

## Selecting the board

```yaml
solution:
  packs:
    - pack: MicroBit::MicroBit_V2_BSP
    - pack: NordicSemiconductor::nRF_DeviceFamilyPack
    - pack: ARM::CMSIS
  target-types:
    - type: microbit-v2
      board: MicroBit::microbit-v2
      target-set:
        - set:
          debugger:
            name: CMSIS-DAP@pyOCD
```

Components: `CMSIS:CORE`, `Device:Startup` (from the DFP), then either `CMSIS Driver:VIO:Board` or the
`Board Support:*` components. With GCC add the linker script and regions header from an example's
`RTE/Device/nRF52833_xxAA/` (see concept-nordic-dfp-cmsis-toolbox).

## Components (Cclass Board Support, header `microbit_v2.h`)

| Cgroup | File | API |
|:--|:--|:--|
| GPIO (required by all) | microbit_gpio.c | `MB_PIN_*` pin map, `mb_gpio_output/input/set/clear/read`, `microbit_delay_us` |
| Time | microbit_time.c | `microbit_time_init/time_ms/delay_ms` (SysTick, defines `SysTick_Handler`) |
| UART | microbit_uart.c | `microbit_uart_init(baud)`, `putc/write/getc/rx_ready`; UARTE0, IRQ receive ring buffer |
| I2C | microbit_i2c.c | `microbit_i2c_init`, `write/read/write_read/read_reg/write_reg`; TWIM0 400 kHz blocking |
| Motion Sensor (requires I2C) | microbit_motion.c | `microbit_motion_init` (WHO_AM_I 0x33/0x40), `accel_read` mg, `mag_read` mgauss, `*_raw` |
| LED Matrix | microbit_display.c | `microbit_display_init/off/clear/set_pixel/get_pixel/show_image/show_char/font`; TIMER1 IRQ, 200 Hz |
| Buttons | microbit_buttons.c | `microbit_button_init`, `microbit_button_pressed(MICROBIT_BUTTON_A/B)` |
| Touch | microbit_touch.c | `microbit_touch_init` (baseline), `logo_raw`, `logo_baseline`, `logo_pressed` (2.5x baseline) |
| Speaker | microbit_speaker.c | `microbit_speaker_init`, `tone(hz)` 31..20000, `off`; PWM0, 1 MHz clock, 50 % duty |
| Microphone | microbit_mic.c | `microbit_mic_init/power`, `sample` 12-bit, `level(n)` peak-to-peak; SAADC AIN3 gain 4 (150 mV range, bias about 75 mV) |
| Temperature | microbit_temp.c | `microbit_temperature_read` in 0.25 C units (nRF TEMP) |

`CMSIS Driver:VIO:Board` (vio_microbit_v2.c) drives ROW1 statically and must not be combined with
`Board Support:LED Matrix`.

## Examples

- `Examples/Blinky`: VIO blinky, bare metal, verified with pyOCD download and breakpoints.
- `Examples/BoardTest`: all components, CMSIS-Compiler STDOUT/STDIN/STDERR Custom retarget to the UART,
  one status line per second, single-key commands (`r`, `t`, `d`, `m`, `h`). Verified output (board on the desk,
  then button A held, then logo touched):
  `temp=21.00 C acc=(955,255,265) mg mag=(166,16207,7015) mG mic_pp=15 btnA=0 btnB=0 logo=194/194`,
  `acc=(-21,-199,-1052) mg mag=(-511,297,490) mG btnA=1`, `logo=20369/194` while touched, `mic_pp=694` on a hand
  clap (gain 4). The 440 Hz tone, the heart and the `!` were confirmed by the user.

## Pack layout and validation

`MicroBit.MicroBit_V2_BSP.pdsc` (schema 1.7.44), `Board/`, `Board/Include/`, `Documents/` (overview and the
CC BY-SA schematic PDF), `Examples/`, `knowledge/`, `gen_pack.sh` (gen-pack 0.14.0, needs GNU coreutils on macOS).
packchk 1.4.5 with the Nordic, CMSIS and CMSIS-Compiler pdsc files: 0 errors, 0 warnings. Board Support
components need a device filter in their condition (`Dname="nRF52833*"`), otherwise packchk warns M336.
