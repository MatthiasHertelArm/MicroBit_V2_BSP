/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * BBC micro:bit V2: internal I2C bus on TWIM0 (SCL P0.08, SDA P0.16, 400 kHz)
 * Blocking EasyDMA transfers. Data to transmit is copied to RAM.
 */
#include <string.h>
#include "microbit_v2.h"

#define MB_I2C_TX_MAX   32U
#define MB_I2C_TIMEOUT  200000U

static uint8_t mb_i2c_txbuf[MB_I2C_TX_MAX];

static void mb_i2c_pin (uint32_t pin) {
  NRF_GPIO_Type *port = mb_gpio_port(pin);
  port->PIN_CNF[MB_PIN_NUM(pin)] = (GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)   |
                                   (GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos) |
                                   (GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)  |
                                   (GPIO_PIN_CNF_DRIVE_S0D1       << GPIO_PIN_CNF_DRIVE_Pos) |
                                   (GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
}

void microbit_i2c_init (void) {
  NRF_TWIM0->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;
  mb_i2c_pin(MB_PIN_I2C_INT_SCL);
  mb_i2c_pin(MB_PIN_I2C_INT_SDA);
  NRF_TWIM0->PSEL.SCL  = MB_PIN_I2C_INT_SCL;
  NRF_TWIM0->PSEL.SDA  = MB_PIN_I2C_INT_SDA;
  NRF_TWIM0->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K400 << TWIM_FREQUENCY_FREQUENCY_Pos;
  NRF_TWIM0->SHORTS    = 0U;
  NRF_TWIM0->INTENCLR  = 0xFFFFFFFFU;
  NRF_TWIM0->ERRORSRC  = NRF_TWIM0->ERRORSRC;
  NRF_TWIM0->EVENTS_STOPPED = 0U;
  NRF_TWIM0->EVENTS_ERROR   = 0U;
  NRF_TWIM0->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;
}

/* Wait for STOPPED; on ERROR issue STOP and report -1 */
static int mb_i2c_wait (void) {
  uint32_t t = MB_I2C_TIMEOUT;
  while ((NRF_TWIM0->EVENTS_STOPPED == 0U) && (NRF_TWIM0->EVENTS_ERROR == 0U) && (t-- != 0U)) {
  }
  if ((NRF_TWIM0->EVENTS_ERROR != 0U) || (t == 0U)) {
    NRF_TWIM0->EVENTS_ERROR = 0U;
    NRF_TWIM0->ERRORSRC = NRF_TWIM0->ERRORSRC;
    NRF_TWIM0->TASKS_STOP = 1U;
    t = MB_I2C_TIMEOUT;
    while ((NRF_TWIM0->EVENTS_STOPPED == 0U) && (t-- != 0U)) {
    }
    NRF_TWIM0->EVENTS_STOPPED = 0U;
    return -1;
  }
  NRF_TWIM0->EVENTS_STOPPED = 0U;
  return 0;
}

int microbit_i2c_write (uint8_t addr, const uint8_t *data, uint32_t len) {
  if ((len == 0U) || (len > MB_I2C_TX_MAX)) {
    return -1;
  }
  memcpy(mb_i2c_txbuf, data, len);
  NRF_TWIM0->ADDRESS    = addr;
  NRF_TWIM0->TXD.PTR    = (uint32_t)mb_i2c_txbuf;
  NRF_TWIM0->TXD.MAXCNT = len;
  NRF_TWIM0->SHORTS     = TWIM_SHORTS_LASTTX_STOP_Msk;
  NRF_TWIM0->EVENTS_STOPPED = 0U;
  NRF_TWIM0->EVENTS_ERROR   = 0U;
  NRF_TWIM0->TASKS_STARTTX  = 1U;
  return mb_i2c_wait();
}

int microbit_i2c_read (uint8_t addr, uint8_t *data, uint32_t len) {
  if (len == 0U) {
    return -1;
  }
  NRF_TWIM0->ADDRESS    = addr;
  NRF_TWIM0->RXD.PTR    = (uint32_t)data;
  NRF_TWIM0->RXD.MAXCNT = len;
  NRF_TWIM0->SHORTS     = TWIM_SHORTS_LASTRX_STOP_Msk;
  NRF_TWIM0->EVENTS_STOPPED = 0U;
  NRF_TWIM0->EVENTS_ERROR   = 0U;
  NRF_TWIM0->TASKS_STARTRX  = 1U;
  return mb_i2c_wait();
}

int microbit_i2c_write_read (uint8_t addr, const uint8_t *wdata, uint32_t wlen, uint8_t *rdata, uint32_t rlen) {
  if ((wlen == 0U) || (wlen > MB_I2C_TX_MAX) || (rlen == 0U)) {
    return -1;
  }
  memcpy(mb_i2c_txbuf, wdata, wlen);
  NRF_TWIM0->ADDRESS    = addr;
  NRF_TWIM0->TXD.PTR    = (uint32_t)mb_i2c_txbuf;
  NRF_TWIM0->TXD.MAXCNT = wlen;
  NRF_TWIM0->RXD.PTR    = (uint32_t)rdata;
  NRF_TWIM0->RXD.MAXCNT = rlen;
  NRF_TWIM0->SHORTS     = TWIM_SHORTS_LASTTX_STARTRX_Msk | TWIM_SHORTS_LASTRX_STOP_Msk;
  NRF_TWIM0->EVENTS_STOPPED = 0U;
  NRF_TWIM0->EVENTS_ERROR   = 0U;
  NRF_TWIM0->TASKS_STARTTX  = 1U;
  return mb_i2c_wait();
}

int microbit_i2c_read_reg (uint8_t addr, uint8_t reg, uint8_t *data, uint32_t len) {
  return microbit_i2c_write_read(addr, &reg, 1U, data, len);
}

int microbit_i2c_write_reg (uint8_t addr, uint8_t reg, uint8_t value) {
  uint8_t buf[2] = { reg, value };
  return microbit_i2c_write(addr, buf, 2U);
}
