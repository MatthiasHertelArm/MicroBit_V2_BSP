# BoardTest project

**BoardTest** exercises every on-board peripheral of the BBC micro:bit V2 through the board support
components of this pack and reports over the USB serial port of the DAPLink interface (115200 baud, 8N1).

## Operation

| Peripheral            | Component                        | Behaviour
|:----------------------|:---------------------------------|:---------------------------------------------------------
| UART (USB serial)     | `Board Support:UART`             | Banner, one status line per second, single-key commands
| LED matrix            | `Board Support:LED Matrix`       | `H`, `I` and a full pattern at start; `A`/`B`/`+` while buttons are pressed; heart while the logo is touched; `!` after a loud sound; otherwise a dot that follows the tilt
| Buttons A and B       | `Board Support:Buttons`          | Shown on the matrix and in the status line
| Touch logo            | `Board Support:Touch`            | Heart on the matrix and a 440 Hz tone while touched; raw charge time in the status line
| Speaker               | `Board Support:Speaker`          | 440 Hz while the logo is touched; command `t` plays 880 Hz for 300 ms
| Microphone            | `Board Support:Microphone`       | Peak-to-peak level in the status line (12-bit, 150 mV range), `!` on the matrix after a loud sound; the microphone LED is on while powered (`m` toggles)
| LSM303AGR             | `Board Support:Motion Sensor`    | WHO_AM_I check at start, acceleration in mg and magnetic field in milligauss in the status line
| Temperature           | `Board Support:Temperature`      | nRF52833 die temperature in the status line
| Time                  | `Board Support:Time`             | SysTick millisecond time base

Serial commands: `r` report now, `t` tone, `d` display test, `m` microphone power, `h` help.

Example status line:

```
t=5021 ms  temp=24.50 C  acc=(-12,8,-1012) mg  mag=(230,-110,-410) mG  mic_pp=38  btnA=0 btnB=0  logo=1180/1150
```

## Build and run

```sh
cbuild BoardTest.csolution.yml --packs --update-rte --context BoardTest.Debug+microbit-v2 --toolchain GCC
```

Program through the on-board DAPLink probe with the Arm CMSIS Debugger (pyOCD, `CMSIS-DAP@pyOCD`), or copy
`out/BoardTest/microbit-v2/Debug/BoardTest.hex` to the `MICROBIT` drive. On a factory-programmed micro:bit the
first pyOCD download fails because of the SoftDevice; see the Blinky README.

Standard I/O is retargeted with CMSIS-Compiler (`STDOUT:Custom`, `STDIN:Custom`, `STDERR:Custom`) to the board
UART in `retarget_stdio.c`.

## Verified on hardware

micro:bit V2.2 (DAPLink 0257), GCC 14.3.1, CMSIS-Toolbox 2.15.0, pyOCD 0.45.1 through the Arm CMSIS Debugger.
Status lines from the session (board flat on the desk, then button A, button B, logo touched, hand clap):

```
LSM303AGR: found (WHO_AM_I 0x33 / 0x40)
Touch logo baseline: 194
Microphone idle level (peak-to-peak): 36
t=2169 ms  temp=22.50 C  acc=(6,-82,-1009) mg  mag=(82,361,430) mG  mic_pp=37  btnA=0 btnB=0  logo=194/194
t=148795 ms  temp=22.50 C  acc=(-9,-189,-1002) mg  mag=(135,357,376) mG  mic_pp=36  btnA=1 btnB=0  logo=193/194
t=150830 ms  temp=22.75 C  acc=(235,-45,-980) mg  mag=(67,313,478) mG  mic_pp=37  btnA=0 btnB=1  logo=193/194
t=154881 ms  temp=22.75 C  acc=(78,-82,-977) mg  mag=(133,312,433) mG  mic_pp=467  btnA=0 btnB=0  logo=21788/194
t=144771 ms  temp=22.75 C  acc=(0,-70,-1020) mg  mag=(111,349,420) mG  mic_pp=694  btnA=0 btnB=0  logo=193/194
```

The 440 Hz tone and the heart appeared while the logo was touched, `!` after the clap, the matrix rows were seen
cycling with the debugger (`mb_row`, `NRF_P0->OUT`), UARTE0 at 0x01D60000 (115200) and TWIM0 at 0x06400000 (400 kHz).
