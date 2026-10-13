---
id: concept-lsm303agr-i2c
type: concept
title: "LSM303AGR over I2C: addresses, registers and scaling as used on the micro:bit V2"
summary: Accelerometer at 0x19 (WHO_AM_I_A 0x0F = 0x33), magnetometer at 0x1E (WHO_AM_I_M 0x4F = 0x40). Multi-byte access needs bit 7 of the sub-address (auto-increment). Accel HR mode +-2 g: 12-bit left-justified, 0.98 mg/digit; magnetometer 1.5 mgauss/LSB. Minimal configuration CTRL_REG1_A=0x57, CTRL_REG4_A=0x88, CFG_REG_A_M=0x80, CFG_REG_B_M=0x02, CFG_REG_C_M=0x10.
domain: board
status: draft
applies_to:
  part: [LSM303AGR]
  board: [microbit-v2]
source: ST LSM303AGR datasheet DocID027765 Rev 11 (pages 13, 38-39, 43-44, 47-49, 52, 61-67), verified on hardware 2026-10-13
license: Apache-2.0
---

## Bus

7-bit addresses 0011001b (0x19) accelerometer and 0011110b (0x1E) magnetometer (datasheet p.39). A read is
SAD+W, SUB, repeated start, SAD+R (p.38). Bit 7 of SUB enables address auto-increment for multi-byte transfers
(p.38); the BSP driver sets it for the 6-byte accelerometer read (`0x28 | 0x80`) and reads the magnetometer from
`0x68` without it, which returned consistent 3-axis data. On the micro:bit the bus has 1 kOhm pull-ups and runs at
400 kHz without error.

## Registers used

| Register | Addr | Value | Meaning |
|:--|:--|:--|:--|
| WHO_AM_I_A | 0x0F | reads 0x33 | identification (p.46) |
| CTRL_REG1_A | 0x20 | 0x57 | ODR 100 Hz, LPen 0, X/Y/Z enabled (p.47) |
| CTRL_REG4_A | 0x23 | 0x88 | BDU 1, FS +-2 g, HR 1 (p.49) |
| OUT_X_L_A .. OUT_Z_H_A | 0x28..0x2D | | two's complement, left-justified (p.52) |
| WHO_AM_I_M | 0x4F | reads 0x40 | identification (p.61) |
| CFG_REG_A_M | 0x60 | 0x80 | COMP_TEMP_EN 1, ODR 10 Hz, continuous mode (p.61) |
| CFG_REG_B_M | 0x61 | 0x02 | offset cancellation (p.63) |
| CFG_REG_C_M | 0x62 | 0x10 | BDU (p.64) |
| OUTX_L_REG_M .. OUTZ_H_REG_M | 0x68..0x6D | | raw field, 1.5 mgauss/LSB (p.13, p.66) |

## Scaling

- Accelerometer HR mode, +-2 g: value = (int16 raw) >> 4, 0.98 mg/digit (p.13). Verified: magnitude of
  (955, 255, 265) mg is 1.02 g with the board tilted.
- Magnetometer: 1.5 mgauss/LSB (p.13). On the test desk one axis read about 16 gauss from a nearby magnetic
  object; held in the air the field magnitude was 0.5 to 0.75 gauss. Use hard-iron calibration for compass use.
- Both interrupt outputs are wired to P0.25 through transistors on the micro:bit; not used by the BSP.
