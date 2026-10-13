/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * BBC micro:bit V2: LSM303AGR accelerometer (0x19) and magnetometer (0x1E) on the internal I2C bus
 * Register and scale information: LSM303AGR datasheet (ST DS11134), register map section 7.
 */
#include "microbit_v2.h"

/* accelerometer registers */
#define LSM_WHO_AM_I_A        0x0FU      /* reads 0x33 */
#define LSM_CTRL_REG1_A       0x20U
#define LSM_CTRL_REG4_A       0x23U
#define LSM_OUT_X_L_A         0x28U
#define LSM_A_AUTOINC         0x80U      /* MSB of the sub-address enables address auto-increment */
/* magnetometer registers */
#define LSM_WHO_AM_I_M        0x4FU      /* reads 0x40 */
#define LSM_CFG_REG_A_M       0x60U
#define LSM_CFG_REG_B_M       0x61U
#define LSM_CFG_REG_C_M       0x62U
#define LSM_OUTX_L_REG_M      0x68U

int microbit_motion_init (void) {
  uint8_t id = 0U;
  int rc = 0;

  /* accelerometer: WHO_AM_I */
  if ((microbit_i2c_read_reg(MB_LSM303AGR_ACC_ADDR, LSM_WHO_AM_I_A, &id, 1U) != 0) || (id != 0x33U)) {
    rc = -1;
  }
  /* 100 Hz, X/Y/Z enabled, normal power; BDU, +-2 g, high resolution (12-bit) */
  if (microbit_i2c_write_reg(MB_LSM303AGR_ACC_ADDR, LSM_CTRL_REG1_A, 0x57U) != 0) { rc = -1; }
  if (microbit_i2c_write_reg(MB_LSM303AGR_ACC_ADDR, LSM_CTRL_REG4_A, 0x88U) != 0) { rc = -1; }

  /* magnetometer: WHO_AM_I */
  id = 0U;
  if ((microbit_i2c_read_reg(MB_LSM303AGR_MAG_ADDR, LSM_WHO_AM_I_M, &id, 1U) != 0) || (id != 0x40U)) {
    rc = -1;
  }
  /* temperature compensation on, 10 Hz, continuous mode; offset cancellation; BDU */
  if (microbit_i2c_write_reg(MB_LSM303AGR_MAG_ADDR, LSM_CFG_REG_A_M, 0x80U) != 0) { rc = -1; }
  if (microbit_i2c_write_reg(MB_LSM303AGR_MAG_ADDR, LSM_CFG_REG_B_M, 0x02U) != 0) { rc = -1; }
  if (microbit_i2c_write_reg(MB_LSM303AGR_MAG_ADDR, LSM_CFG_REG_C_M, 0x10U) != 0) { rc = -1; }

  return rc;
}

int microbit_accel_read_raw (microbit_vec3_t *raw) {
  uint8_t d[6];
  if (microbit_i2c_read_reg(MB_LSM303AGR_ACC_ADDR, LSM_OUT_X_L_A | LSM_A_AUTOINC, d, 6U) != 0) {
    return -1;
  }
  raw->x = (int16_t)((uint16_t)d[0] | ((uint16_t)d[1] << 8));
  raw->y = (int16_t)((uint16_t)d[2] | ((uint16_t)d[3] << 8));
  raw->z = (int16_t)((uint16_t)d[4] | ((uint16_t)d[5] << 8));
  return 0;
}

int microbit_accel_read (microbit_vec3_t *mg) {
  microbit_vec3_t raw;
  if (microbit_accel_read_raw(&raw) != 0) {
    return -1;
  }
  /* high-resolution mode: 12-bit left-justified, 0.98 mg/digit at +-2 g */
  mg->x = (int16_t)(((int32_t)(raw.x >> 4) * 98) / 100);
  mg->y = (int16_t)(((int32_t)(raw.y >> 4) * 98) / 100);
  mg->z = (int16_t)(((int32_t)(raw.z >> 4) * 98) / 100);
  return 0;
}

int microbit_mag_read_raw (microbit_vec3_t *raw) {
  uint8_t d[6];
  if (microbit_i2c_read_reg(MB_LSM303AGR_MAG_ADDR, LSM_OUTX_L_REG_M, d, 6U) != 0) {
    return -1;
  }
  raw->x = (int16_t)((uint16_t)d[0] | ((uint16_t)d[1] << 8));
  raw->y = (int16_t)((uint16_t)d[2] | ((uint16_t)d[3] << 8));
  raw->z = (int16_t)((uint16_t)d[4] | ((uint16_t)d[5] << 8));
  return 0;
}

int microbit_mag_read (microbit_vec3_t *mgauss) {
  microbit_vec3_t raw;
  if (microbit_mag_read_raw(&raw) != 0) {
    return -1;
  }
  /* 1.5 mgauss/LSB */
  mgauss->x = (int16_t)(((int32_t)raw.x * 3) / 2);
  mgauss->y = (int16_t)(((int32_t)raw.y * 3) / 2);
  mgauss->z = (int16_t)(((int32_t)raw.z * 3) / 2);
  return 0;
}
