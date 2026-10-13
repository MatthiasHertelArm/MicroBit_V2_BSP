/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * BBC micro:bit V2: nRF52833 die temperature sensor
 */
#include "microbit_v2.h"

int32_t microbit_temperature_read (void) {
  int32_t t;
  uint32_t timeout = 100000U;

  NRF_TEMP->EVENTS_DATARDY = 0U;
  NRF_TEMP->TASKS_START = 1U;
  while ((NRF_TEMP->EVENTS_DATARDY == 0U) && (timeout-- != 0U)) {
  }
  t = NRF_TEMP->TEMP;                 /* 0.25 degree Celsius units */
  NRF_TEMP->EVENTS_DATARDY = 0U;
  NRF_TEMP->TASKS_STOP = 1U;
  return t;
}
