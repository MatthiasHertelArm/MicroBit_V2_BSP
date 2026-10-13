/******************************************************************************
 * @file     microbit_v2.h
 * @brief    BBC micro:bit V2 board support: pin map and peripheral drivers
 * @version  V1.0.0
 * @date     13. October 2026
 ******************************************************************************/
/*
 * Copyright (c) 2026 Arm Limited (or its affiliates).
 * All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MICROBIT_V2_H
#define MICROBIT_V2_H

#include <stdint.h>
#include <stdbool.h>
#include "nrf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
  Pin map (micro:bit V2.2.1 schematic, sheets 2 and 3)
  A pin number is 32 * port + pin, as used by the Nordic MDK (NRF_GPIO_PIN_MAP).
 *============================================================================*/
#define MB_PIN(port, pin)       ((uint32_t)(((port) << 5) | ((pin) & 31U)))
#define MB_PIN_PORT(p)          ((p) >> 5)
#define MB_PIN_NUM(p)           ((p) & 31U)

/* LED matrix: LED lights when its ROW is high and its COL is low */
#define MB_PIN_ROW1             MB_PIN(0, 21)
#define MB_PIN_ROW2             MB_PIN(0, 22)
#define MB_PIN_ROW3             MB_PIN(0, 15)
#define MB_PIN_ROW4             MB_PIN(0, 24)
#define MB_PIN_ROW5             MB_PIN(0, 19)
#define MB_PIN_COL1             MB_PIN(0, 28)
#define MB_PIN_COL2             MB_PIN(0, 11)
#define MB_PIN_COL3             MB_PIN(0, 31)
#define MB_PIN_COL4             MB_PIN(1,  5)
#define MB_PIN_COL5             MB_PIN(0, 30)

/* Buttons: active low, 10 kOhm pull-ups on the board */
#define MB_PIN_BTN_A            MB_PIN(0, 14)
#define MB_PIN_BTN_B            MB_PIN(0, 23)

/* Capacitive touch logo (10 MOhm pull-up to VREG on the board) */
#define MB_PIN_LOGO             MB_PIN(1,  4)

/* Speaker MLT-8530, driven through a MOSFET (gate on this pin) */
#define MB_PIN_SPEAKER          MB_PIN(0,  0)

/* Microphone Knowles SPU0410LR5H-QB-7 (analog): output on AIN3, power from RUN_MIC */
#define MB_PIN_MIC_IN           MB_PIN(0,  5)
#define MB_PIN_RUN_MIC          MB_PIN(0, 20)

/* Internal I2C bus: LSM303AGR motion sensor and interface MCU, 1 kOhm pull-ups */
#define MB_PIN_I2C_INT_SCL      MB_PIN(0,  8)
#define MB_PIN_I2C_INT_SDA      MB_PIN(0, 16)
#define MB_PIN_I2C_INT_INT      MB_PIN(0, 25)

/* UART to the DAPLink interface MCU (USB serial on the host) */
#define MB_PIN_UART_TX          MB_PIN(0,  6)
#define MB_PIN_UART_RX          MB_PIN(1,  8)

/* Edge connector */
#define MB_PIN_P0               MB_PIN(0,  2)   /* ring 0, AIN0, touch */
#define MB_PIN_P1               MB_PIN(0,  3)   /* ring 1, AIN1, touch */
#define MB_PIN_P2               MB_PIN(0,  4)   /* ring 2, AIN2, touch */
#define MB_PIN_P3               MB_PIN(0, 31)   /* COL3, AIN7 */
#define MB_PIN_P4               MB_PIN(0, 28)   /* COL1, AIN4 */
#define MB_PIN_P5               MB_PIN(0, 14)   /* button A */
#define MB_PIN_P6               MB_PIN(1,  5)   /* COL4 */
#define MB_PIN_P7               MB_PIN(0, 11)   /* COL2 */
#define MB_PIN_P8               MB_PIN(0, 10)   /* NFC2 */
#define MB_PIN_P9               MB_PIN(0,  9)   /* NFC1 */
#define MB_PIN_P10              MB_PIN(0, 30)   /* COL5, AIN6 */
#define MB_PIN_P11              MB_PIN(0, 23)   /* button B */
#define MB_PIN_P12              MB_PIN(0, 12)   /* reserved for accessibility */
#define MB_PIN_P13              MB_PIN(0, 17)   /* SPI_EXT_SCK */
#define MB_PIN_P14              MB_PIN(0,  1)   /* SPI_EXT_MISO */
#define MB_PIN_P15              MB_PIN(0, 13)   /* SPI_EXT_MOSI */
#define MB_PIN_P16              MB_PIN(1,  2)   /* GPIO3 */
#define MB_PIN_P19              MB_PIN(0, 26)   /* I2C_EXT_SCL, 4.7 kOhm pull-up */
#define MB_PIN_P20              MB_PIN(1,  0)   /* I2C_EXT_SDA, 4.7 kOhm pull-up */

/* LSM303AGR I2C addresses (7-bit) */
#define MB_LSM303AGR_ACC_ADDR   0x19U
#define MB_LSM303AGR_MAG_ADDR   0x1EU

/*============================================================================
  GPIO helpers
 *============================================================================*/
static inline NRF_GPIO_Type *mb_gpio_port (uint32_t pin) {
  return (MB_PIN_PORT(pin) == 0U) ? NRF_P0 : NRF_P1;
}

/* Configure as push-pull output (standard or high drive) with initial level */
void     mb_gpio_output   (uint32_t pin, uint32_t level, bool high_drive);
/* Configure as input: pull = GPIO_PIN_CNF_PULL_Disabled/Pulldown/Pullup */
void     mb_gpio_input    (uint32_t pin, uint32_t pull);
/* Disconnect the input buffer (lowest power, pin floating) */
void     mb_gpio_disconnect (uint32_t pin);
static inline void mb_gpio_set   (uint32_t pin) { mb_gpio_port(pin)->OUTSET = 1UL << MB_PIN_NUM(pin); }
static inline void mb_gpio_clear (uint32_t pin) { mb_gpio_port(pin)->OUTCLR = 1UL << MB_PIN_NUM(pin); }
static inline void mb_gpio_write (uint32_t pin, uint32_t level) { if (level) mb_gpio_set(pin); else mb_gpio_clear(pin); }
static inline uint32_t mb_gpio_read (uint32_t pin) { return (mb_gpio_port(pin)->IN >> MB_PIN_NUM(pin)) & 1UL; }

/*============================================================================
  Time (SysTick, 1 ms) — component Board Support:Time
 *============================================================================*/
void     microbit_time_init  (void);
uint32_t microbit_time_ms    (void);
void     microbit_delay_ms   (uint32_t ms);
void     microbit_delay_us   (uint32_t us);      /* busy loop, no SysTick needed */

/*============================================================================
  UART to the interface MCU (USB serial) — component Board Support:UART
  UARTE0, TX P0.06, RX P1.08, 8N1, no flow control. Interrupt driven receive.
 *============================================================================*/
void     microbit_uart_init     (uint32_t baudrate);     /* 1200 .. 1000000, standard rates */
void     microbit_uart_putc     (char c);
void     microbit_uart_write    (const char *s);
int      microbit_uart_getc     (void);                  /* -1 when no byte is available */
bool     microbit_uart_rx_ready (void);

/*============================================================================
  Internal I2C bus — component Board Support:I2C (TWIM0, 400 kHz)
  Return 0 on success, -1 on NACK or bus error.
 *============================================================================*/
void     microbit_i2c_init       (void);
int      microbit_i2c_write      (uint8_t addr, const uint8_t *data, uint32_t len);
int      microbit_i2c_read       (uint8_t addr, uint8_t *data, uint32_t len);
int      microbit_i2c_write_read (uint8_t addr, const uint8_t *wdata, uint32_t wlen, uint8_t *rdata, uint32_t rlen);
int      microbit_i2c_read_reg   (uint8_t addr, uint8_t reg, uint8_t *data, uint32_t len);
int      microbit_i2c_write_reg  (uint8_t addr, uint8_t reg, uint8_t value);

/*============================================================================
  Motion sensor LSM303AGR — component Board Support:Motion Sensor
 *============================================================================*/
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} microbit_vec3_t;

int      microbit_motion_init  (void);                           /* 0: both WHO_AM_I ok */
int      microbit_accel_read   (microbit_vec3_t *mg);            /* milli-g, +-2 g, 100 Hz */
int      microbit_mag_read     (microbit_vec3_t *mgauss);        /* milligauss, 10 Hz */
int      microbit_accel_read_raw (microbit_vec3_t *raw);         /* 12-bit left-justified in 16 bits */
int      microbit_mag_read_raw   (microbit_vec3_t *raw);         /* 1.5 mgauss/LSB */

/*============================================================================
  5x5 LED matrix — component Board Support:LED Matrix
  Refreshed row by row from the TIMER1 interrupt (one row per millisecond).
  x: column 0..4 (left to right), y: row 0..4 (top to bottom).
  Not to be used together with the CMSIS-Driver VIO (both drive the matrix).
 *============================================================================*/
void     microbit_display_init       (void);
void     microbit_display_off        (void);
void     microbit_display_clear      (void);
void     microbit_display_set_pixel  (uint32_t x, uint32_t y, bool on);
bool     microbit_display_get_pixel  (uint32_t x, uint32_t y);
void     microbit_display_show_image (const uint8_t rows[5]);     /* bit 4 = left column */
void     microbit_display_show_char  (char c);                    /* 5x5 font, ASCII 32..90, lowercase mapped */
const uint8_t *microbit_display_font (char c);                    /* 5 row bytes of the glyph */

/*============================================================================
  Buttons A and B — component Board Support:Buttons
 *============================================================================*/
typedef enum {
  MICROBIT_BUTTON_A = 0,
  MICROBIT_BUTTON_B = 1
} microbit_button_t;

void     microbit_button_init    (void);
bool     microbit_button_pressed (microbit_button_t button);

/*============================================================================
  Capacitive touch logo — component Board Support:Touch
  Measures the charge time of the pad through the 10 MOhm pull-up on the board.
 *============================================================================*/
void     microbit_touch_init         (void);                 /* measures the untouched baseline */
uint32_t microbit_touch_logo_raw     (void);                 /* charge time in loop counts */
uint32_t microbit_touch_logo_baseline(void);
bool     microbit_touch_logo_pressed (void);

/*============================================================================
  Speaker — component Board Support:Speaker (PWM0 on P0.00)
 *============================================================================*/
void     microbit_speaker_init (void);
void     microbit_speaker_tone (uint32_t frequency_hz);      /* 31 .. 20000 Hz, 0 = off */
void     microbit_speaker_off  (void);

/*============================================================================
  Microphone — component Board Support:Microphone (SAADC AIN3, RUN_MIC)
  The microphone LED on the front lights while the microphone is powered.
 *============================================================================*/
void     microbit_mic_init   (void);                         /* powers the microphone */
void     microbit_mic_power  (bool on);
uint16_t microbit_mic_sample (void);                         /* 12-bit, 0..4095, 150 mV full scale, idle about 2000 */
uint16_t microbit_mic_level  (uint32_t samples);             /* peak-to-peak of n samples */

/*============================================================================
  Temperature — component Board Support:Temperature (nRF52833 die sensor)
 *============================================================================*/
int32_t  microbit_temperature_read (void);                   /* in 0.25 degree Celsius units */

#ifdef __cplusplus
}
#endif

#endif /* MICROBIT_V2_H */
