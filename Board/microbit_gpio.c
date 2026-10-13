/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * BBC micro:bit V2: GPIO helpers and busy-wait delay
 */
#include "microbit_v2.h"

void mb_gpio_output (uint32_t pin, uint32_t level, bool high_drive) {
  NRF_GPIO_Type *port = mb_gpio_port(pin);
  uint32_t drive = high_drive ? GPIO_PIN_CNF_DRIVE_H0H1 : GPIO_PIN_CNF_DRIVE_S0S1;

  mb_gpio_write(pin, level);
  port->PIN_CNF[MB_PIN_NUM(pin)] = (GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)   |
                                   (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
                                   (GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)  |
                                   (drive                         << GPIO_PIN_CNF_DRIVE_Pos) |
                                   (GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
}

void mb_gpio_input (uint32_t pin, uint32_t pull) {
  NRF_GPIO_Type *port = mb_gpio_port(pin);

  port->PIN_CNF[MB_PIN_NUM(pin)] = (GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)   |
                                   (GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos) |
                                   (pull                          << GPIO_PIN_CNF_PULL_Pos)  |
                                   (GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos) |
                                   (GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
}

void mb_gpio_disconnect (uint32_t pin) {
  NRF_GPIO_Type *port = mb_gpio_port(pin);

  port->PIN_CNF[MB_PIN_NUM(pin)] = (GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)   |
                                   (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
                                   (GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos);
}

/* Busy wait: 64 MHz core, about 4 cycles per iteration */
void microbit_delay_us (uint32_t us) {
  volatile uint32_t n = us * 16U;
  while (n-- != 0U) {
    __NOP();
  }
}
