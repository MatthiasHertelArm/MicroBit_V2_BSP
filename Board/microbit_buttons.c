/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * BBC micro:bit V2: buttons A (P0.14) and B (P0.23), active low, external pull-ups
 */
#include "microbit_v2.h"

static const uint32_t mb_button_pin[2] = { MB_PIN_BTN_A, MB_PIN_BTN_B };

void microbit_button_init (void) {
  mb_gpio_input(MB_PIN_BTN_A, GPIO_PIN_CNF_PULL_Disabled);
  mb_gpio_input(MB_PIN_BTN_B, GPIO_PIN_CNF_PULL_Disabled);
}

bool microbit_button_pressed (microbit_button_t button) {
  if ((uint32_t)button > 1U) {
    return false;
  }
  return (mb_gpio_read(mb_button_pin[button]) == 0U);
}
