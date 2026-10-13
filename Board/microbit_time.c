/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * BBC micro:bit V2: millisecond time base on SysTick
 */
#include "microbit_v2.h"

static volatile uint32_t mb_ticks;

void SysTick_Handler (void) {
  mb_ticks++;
}

void microbit_time_init (void) {
  SystemCoreClockUpdate();
  (void)SysTick_Config(SystemCoreClock / 1000U);
}

uint32_t microbit_time_ms (void) {
  return mb_ticks;
}

void microbit_delay_ms (uint32_t ms) {
  uint32_t start = mb_ticks;
  while ((mb_ticks - start) < ms) {
    __WFI();
  }
}
