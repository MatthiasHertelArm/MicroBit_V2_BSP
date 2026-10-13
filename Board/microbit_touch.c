/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * BBC micro:bit V2: capacitive touch logo on P1.04
 * The pad is pulled up by 10 MOhm on the board (R53). The pin is driven low to
 * discharge the pad, released as an input and the time until it reads high is
 * counted. A finger adds capacitance and lengthens the charge time.
 */
#include "microbit_v2.h"

#define MB_TOUCH_MAX_COUNT    40000U
#define MB_TOUCH_SAMPLES      8U

static uint32_t mb_touch_baseline;

uint32_t microbit_touch_logo_raw (void) {
  NRF_GPIO_Type *port = mb_gpio_port(MB_PIN_LOGO);
  uint32_t mask = 1UL << MB_PIN_NUM(MB_PIN_LOGO);
  uint32_t count = 0U;

  /* discharge */
  mb_gpio_output(MB_PIN_LOGO, 0U, false);
  microbit_delay_us(100U);

  /* release and time the charge through the external pull-up */
  __disable_irq();
  port->PIN_CNF[MB_PIN_NUM(MB_PIN_LOGO)] = (GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos)   |
                                           (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos) |
                                           (GPIO_PIN_CNF_PULL_Disabled  << GPIO_PIN_CNF_PULL_Pos);
  while (((port->IN & mask) == 0U) && (count < MB_TOUCH_MAX_COUNT)) {
    count++;
  }
  __enable_irq();

  return count;
}

void microbit_touch_init (void) {
  uint32_t sum = 0U;
  uint32_t n;

  for (n = 0U; n < MB_TOUCH_SAMPLES; n++) {
    sum += microbit_touch_logo_raw();
    microbit_delay_us(500U);
  }
  mb_touch_baseline = sum / MB_TOUCH_SAMPLES;
  if (mb_touch_baseline < 4U) {
    mb_touch_baseline = 4U;
  }
}

uint32_t microbit_touch_logo_baseline (void) {
  return mb_touch_baseline;
}

bool microbit_touch_logo_pressed (void) {
  uint32_t raw = microbit_touch_logo_raw();
  return (raw > (mb_touch_baseline * 2U + mb_touch_baseline / 2U));   /* 2.5x the untouched charge time */
}
