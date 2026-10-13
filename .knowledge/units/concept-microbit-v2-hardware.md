---
id: concept-microbit-v2-hardware
type: concept
title: "BBC micro:bit V2 hardware: parts, pin map and electrical notes"
summary: "nRF52833-QIAA target (Cortex-M4F, 512 KB Flash, 128 KB RAM) with an nRF52820 DAPLink interface MCU, LSM303AGR motion sensor on internal I2C, analog microphone, speaker on P0.00, 5x5 LED matrix, buttons, touch logo; pin map and electrical notes."
domain: board
status: draft
applies_to:
  board: [microbit-v2]
  device: [nRF52833_xxAA]
source: MicroBit_V2.2.1_nRF52820 schematic (Micro:bit Educational Foundation, CC BY-SA 4.0), tech.microbit.org/hardware/schematic and /edgeconnector, Zephyr boards/bbc/microbit_v2, codal-microbit-v2 model/MicroBitIO.cpp; verified on hardware 2026-10-13
license: Apache-2.0 (text); schematic CC BY-SA 4.0
---

## Parts (V2.2.1)

| Function | Part | Notes |
|:--|:--|:--|
| Target MCU (U2) | Nordic nRF52833-QIAA | aQFN73, 32 MHz crystal X1, 32.768 kHz not fitted (internal RC LFCLK) |
| Interface MCU (U5) | Nordic nRF52820-QDAA | DAPLink: CMSIS-DAP HID/WebUSB, USB serial, MSD; V2.0 boards use NXP KL27Z |
| Motion sensor (U6/U1) | ST LSM303AGR | I2C 0x19 accel, 0x1E mag; INT1_XL and INT_MAG/DRDY wired to I2C_INT_INT P0.25 through open-drain transistors |
| Microphone (U4) | Knowles SPU0410LR5H-QB-7 | analog, output biased by R62/R63, 1 uF to MIC_IN; powered by RUN_MIC through 120 Ohm/10 uF; D70 (red) lights while powered |
| Speaker (SP1) | MLT-8530 | gate of MOSFET T2 driven from SPEAKER P0.00 (4.7 kOhm series, 100 kOhm pull-down); interface MCU can also drive it (IF_SPEAKER) |
| Regulator (U3) | onsemi NCP114BMX330TCG | 3.3 V from VCOMBINED (USB via BAT60J or battery via BAT60A diodes) |
| Battery (J2) | JST PH 2-pin | 2x AAA, 3 V |

## Pin map (nRF52833 side)

| Signal | Pin | Signal | Pin |
|:--|:--|:--|:--|
| ROW1..ROW5 | P0.21, P0.22, P0.15, P0.24, P0.19 | COL1..COL5 | P0.28, P0.11, P0.31, P1.05, P0.30 |
| BTN_A / BTN_B | P0.14 / P0.23 (active low, 10 kOhm pull-ups R4/R5) | LOGO (touch) | P1.04 (R47 0 Ohm, R53 10 MOhm to VREG) |
| SPEAKER | P0.00 | MIC_IN / RUN_MIC | P0.05 (AIN3) / P0.20 |
| I2C_INT_SCL / SDA / INT | P0.08 / P0.16 / P0.25 (1 kOhm pull-ups R51/R52) | I2C_EXT_SCL / SDA | P0.26 (P19) / P1.00 (P20), 4.7 kOhm pull-ups |
| UART TX (nRF52833 -> DAPLink) | P0.06 | UART RX (DAPLink -> nRF52833) | P1.08 |
| SPI_EXT SCK / MISO / MOSI | P0.17 (P13) / P0.01 (P14) / P0.13 (P15) | RING0..2 | P0.02 (P0, AIN0), P0.03 (P1, AIN1), P0.04 (P2, AIN2), 10 MOhm pull-ups |
| Edge P8 / P9 | P0.10 (NFC2) / P0.09 (NFC1) | Edge P12 / P16 | P0.12 / P1.02 |
| SWDIO / SWDCLK | TP12 / TP11 from the interface MCU | nRESET | P0.18 |

The schematic names the UART nets from the interface-MCU side (`UART_INT_RX` on nRF52833 P0.06 is the interface's
receive line). On the nRF52833, P0.06 is the transmitter: confirmed on hardware with the UART driver of the BSP.

## Electrical notes

- LED matrix: rows source, columns sink (LED on = ROW high, COL low). COL1/3/5 are on ADC-capable pins for light
  sensing. One row must be driven at a time; static drive of a single row works without refresh.
- Buttons: external pull-ups, so no internal pull is needed; both are also edge-connector pins P5 and P11.
- Touch logo: no external circuit besides the 10 MOhm pull-up. Charge-time measurement works: discharge the pin
  low, release as input without pull, count until high. Untouched about 190 loop counts at 64 MHz on the tested
  board, several times more when touched.
- Microphone: analog MEMS output, no amplifier; R62/R63 bias the output to about 75 mV (73 mV measured through
  the SAADC), which is the middle of the SAADC gain-4 range (0 to 150 mV with the internal 0.6 V reference). With
  gain 1/6 (3.6 V range) the signal is only a few tens of LSB. With gain 4 and 12 bits: idle peak-to-peak about
  36 LSB, speech at arm's length 300 to 1000 LSB, a hand clap about 700 LSB.
- Magnetometer: lying on the test desk the sensor read about 16 gauss on one axis (environment: a magnet or a
  steel object nearby); held in the air it read a field of 0.5 to 0.75 gauss magnitude. Do not take a large static
  reading for a sensor fault; move the board and use hard-iron calibration before compass use.
- nRF52833 die temperature reads about 20.5 C at room temperature; LSM303AGR temperature is relative only.
- Interface MCU trace pins: P0.07 TRACECLK, P0.11 TRACEDATA2 (COL2), P0.12 TRACEDATA1, P1.00 TRACEDATA0 (P20),
  P1.09 TRACEDATA3. SystemInit applies errata 32 (debug session enabling trace pins); not an issue in practice.
