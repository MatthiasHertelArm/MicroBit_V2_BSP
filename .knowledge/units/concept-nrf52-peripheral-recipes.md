---
id: concept-nrf52-peripheral-recipes
type: concept
title: "nRF52833 bare-metal peripheral recipes verified on the micro:bit V2 (UARTE, TWIM, SAADC, PWM, TIMER, TEMP, GPIO)"
summary: "Register sequences of the BSP drivers that ran on hardware: UARTE continuous receive, TWIM with SHORTS, SAADC one-shot, PWM tones, TIMER1 1 ms interrupt, TEMP; EasyDMA pointers must point to RAM."
domain: device
status: draft
applies_to:
  device: [nRF52833_xxAA, nRF52840_xxAA, nRF52832_xxAA]
source: BSP drivers Board/microbit_*.c, nrf52833_bitfields.h, verified 2026-10-13
license: Apache-2.0
---

## General

- EasyDMA (`TXD.PTR`, `RXD.PTR`, `RESULT.PTR`, `SEQ[n].PTR`) only accepts RAM addresses; copy constant data to a
  RAM buffer first. A pointer into Flash silently transfers garbage.
- Event registers are cleared by writing 0; tasks are triggered by writing 1.
- Pins are given as 32 * port + pin in `PSEL` registers (P1.08 = 40).
- Peripheral IRQ handler names come from the Nordic startup file, e.g. `UARTE0_UART0_IRQHandler`, `TIMER1_IRQHandler`.

## UARTE0 (115200, polled TX, IRQ RX)

`PSEL.TXD/RXD`, `BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud115200` (0x01D60000 read back), `CONFIG = 0` (no HWFC,
no parity), `RXD.PTR/MAXCNT = 1`, `SHORTS = ENDRX_STARTRX`, `INTENSET = ENDRX`, `ENABLE = 8`, `TASKS_STARTRX`.
Transmit: `TXD.PTR/MAXCNT = 1`, `TASKS_STARTTX`, wait `EVENTS_ENDTX`, `TASKS_STOPTX`. The TX pin must be configured
as output high before enabling.

## TWIM0 (400 kHz)

Pins as input with pull-up and `S0D1` drive. `FREQUENCY = K400` (0x06400000), `ENABLE = 6`.
Write: `ADDRESS`, `TXD.PTR/MAXCNT`, `SHORTS = LASTTX_STOP`, `TASKS_STARTTX`, wait `EVENTS_STOPPED` or
`EVENTS_ERROR`. Write-then-read: `SHORTS = LASTTX_STARTRX | LASTRX_STOP`, `TASKS_STARTTX`. Read:
`SHORTS = LASTRX_STOP`, `TASKS_STARTRX`. On error: clear `ERRORSRC` by writing it back, `TASKS_STOP`, wait STOPPED.

## SAADC one-shot (AIN3, 12-bit)

`RESOLUTION = 12bit`, `CH[0].PSELP = AnalogInput3`, `CH[0].CONFIG = RESP/RESN bypass, GAIN (1/6 for 3.6 V full
scale, 4 for 150 mV full scale as the micro:bit microphone needs), REFSEL internal (0.6 V), TACQ 10..20 us, SE`, `RESULT.PTR/MAXCNT = 1`, `ENABLE = 1`. Per sample: `TASKS_START`, wait `EVENTS_STARTED`,
`TASKS_SAMPLE`, wait `EVENTS_END`, `TASKS_STOP`, wait `EVENTS_STOPPED`. Result is int16; clamp negatives to 0.

## PWM0 square wave (speaker)

`PRESCALER = DIV_16` (1 MHz), `MODE = Up`, `DECODER = Common | RefreshCount`, `COUNTERTOP = 1e6 / f` (max 0x7FFF),
sequence value = `COUNTERTOP / 2` with bit 15 = 0, `SEQ[0]` and `SEQ[1]` with CNT 1, `LOOP = 1`,
`SHORTS = LOOPSDONE_SEQSTART0` for a continuous tone, `ENABLE = 1`, `TASKS_SEQSTART[0]`. Stop: `TASKS_STOP`, wait
`EVENTS_STOPPED`, `ENABLE = 0`, drive the pin low.

## TIMER1 periodic interrupt (1 ms)

`MODE = Timer`, `BITMODE = 16Bit`, `PRESCALER = 4` (16 MHz / 16), `CC[0] = 1000`, `SHORTS = COMPARE0_CLEAR`,
`INTENSET = COMPARE0`, `NVIC_EnableIRQ(TIMER1_IRQn)`, `TASKS_START`. Clear `EVENTS_COMPARE[0]` in the handler.
Verified `CC[0] = 1000` and row counter advancing on the target.

## TEMP

`TASKS_START`, wait `EVENTS_DATARDY`, read `TEMP` (int32, 0.25 C units, about 82..84 at 20.5..21 C), clear the
event, `TASKS_STOP`.

## GPIO

`PIN_CNF[n]` = DIR | INPUT (connect for inputs) | PULL | DRIVE (`H0H1` when the pin powers something, as RUN_MIC
does for the microphone) | SENSE. `OUTSET`/`OUTCLR`/`IN` on `NRF_P0`/`NRF_P1`.
