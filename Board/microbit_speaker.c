/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * BBC micro:bit V2: speaker MLT-8530 on P0.00 through a MOSFET, square wave from PWM0
 */
#include "microbit_v2.h"

static uint16_t mb_speaker_duty[2];          /* PWM sequence values, must be in RAM */

void microbit_speaker_init (void) {
  mb_gpio_output(MB_PIN_SPEAKER, 0U, false);
  NRF_PWM0->ENABLE = PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos;
  NRF_PWM0->PSEL.OUT[0] = MB_PIN_SPEAKER;
  NRF_PWM0->PSEL.OUT[1] = PWM_PSEL_OUT_CONNECT_Disconnected << PWM_PSEL_OUT_CONNECT_Pos;
  NRF_PWM0->PSEL.OUT[2] = PWM_PSEL_OUT_CONNECT_Disconnected << PWM_PSEL_OUT_CONNECT_Pos;
  NRF_PWM0->PSEL.OUT[3] = PWM_PSEL_OUT_CONNECT_Disconnected << PWM_PSEL_OUT_CONNECT_Pos;
  NRF_PWM0->MODE      = PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos;
  NRF_PWM0->PRESCALER = PWM_PRESCALER_PRESCALER_DIV_16 << PWM_PRESCALER_PRESCALER_Pos;   /* 1 MHz */
  NRF_PWM0->DECODER   = (PWM_DECODER_LOAD_Common       << PWM_DECODER_LOAD_Pos) |
                        (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
  NRF_PWM0->LOOP      = 1U;                                   /* play SEQ0, SEQ1, then LOOPSDONE */
  NRF_PWM0->SHORTS    = PWM_SHORTS_LOOPSDONE_SEQSTART0_Msk;   /* restart forever */
  NRF_PWM0->SEQ[0].PTR      = (uint32_t)&mb_speaker_duty[0];
  NRF_PWM0->SEQ[0].CNT      = 1U;
  NRF_PWM0->SEQ[0].REFRESH  = 0U;
  NRF_PWM0->SEQ[0].ENDDELAY = 0U;
  NRF_PWM0->SEQ[1].PTR      = (uint32_t)&mb_speaker_duty[1];
  NRF_PWM0->SEQ[1].CNT      = 1U;
  NRF_PWM0->SEQ[1].REFRESH  = 0U;
  NRF_PWM0->SEQ[1].ENDDELAY = 0U;
}

void microbit_speaker_tone (uint32_t frequency_hz) {
  uint32_t top;

  if (frequency_hz == 0U) {
    microbit_speaker_off();
    return;
  }
  if (frequency_hz < 31U) {
    frequency_hz = 31U;
  }
  if (frequency_hz > 20000U) {
    frequency_hz = 20000U;
  }
  top = 1000000U / frequency_hz;                 /* 1 MHz PWM clock, 15-bit counter top */
  if (top > 0x7FFFU) {
    top = 0x7FFFU;
  }

  mb_speaker_duty[0] = (uint16_t)(top / 2U);     /* 50 % duty, polarity bit 15 = 0 */
  mb_speaker_duty[1] = mb_speaker_duty[0];

  NRF_PWM0->ENABLE = PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos;
  NRF_PWM0->COUNTERTOP = top;
  NRF_PWM0->EVENTS_STOPPED = 0U;
  NRF_PWM0->TASKS_SEQSTART[0] = 1U;
}

void microbit_speaker_off (void) {
  uint32_t timeout = 100000U;

  if ((NRF_PWM0->ENABLE & PWM_ENABLE_ENABLE_Msk) != 0U) {
    NRF_PWM0->EVENTS_STOPPED = 0U;
    NRF_PWM0->TASKS_STOP = 1U;
    while ((NRF_PWM0->EVENTS_STOPPED == 0U) && (timeout-- != 0U)) {
    }
    NRF_PWM0->ENABLE = PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos;
  }
  mb_gpio_output(MB_PIN_SPEAKER, 0U, false);
}
