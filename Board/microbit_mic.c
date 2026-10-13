/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * BBC micro:bit V2: analog MEMS microphone Knowles SPU0410LR5H-QB-7
 * Output MIC_IN on P0.05 / AIN3, DC biased by R62/R63 to about 75 mV (measured 73 mV), which fits the
 * SAADC gain-4 range of 0 .. 150 mV (internal 0.6 V reference). Power from RUN_MIC P0.20, which also
 * lights the microphone LED on the front of the board.
 */
#include "microbit_v2.h"

static volatile int16_t mb_mic_result;      /* SAADC result, RAM */

void microbit_mic_power (bool on) {
  mb_gpio_output(MB_PIN_RUN_MIC, on ? 1U : 0U, true);   /* high drive: the pin powers the microphone */
}

void microbit_mic_init (void) {
  microbit_mic_power(true);

  NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos;
  NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_12bit << SAADC_RESOLUTION_VAL_Pos;
  NRF_SAADC->OVERSAMPLE = SAADC_OVERSAMPLE_OVERSAMPLE_Bypass << SAADC_OVERSAMPLE_OVERSAMPLE_Pos;
  NRF_SAADC->SAMPLERATE = SAADC_SAMPLERATE_MODE_Task << SAADC_SAMPLERATE_MODE_Pos;

  NRF_SAADC->CH[0].PSELP  = SAADC_CH_PSELP_PSELP_AnalogInput3 << SAADC_CH_PSELP_PSELP_Pos;   /* P0.05 */
  NRF_SAADC->CH[0].PSELN  = SAADC_CH_PSELN_PSELN_NC << SAADC_CH_PSELN_PSELN_Pos;
  NRF_SAADC->CH[0].CONFIG = (SAADC_CH_CONFIG_RESP_Bypass     << SAADC_CH_CONFIG_RESP_Pos)   |
                            (SAADC_CH_CONFIG_RESN_Bypass     << SAADC_CH_CONFIG_RESN_Pos)   |
                            (SAADC_CH_CONFIG_GAIN_Gain4      << SAADC_CH_CONFIG_GAIN_Pos)   |
                            (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) |  /* 0.6 V / 4 = 150 mV full scale */
                            (SAADC_CH_CONFIG_TACQ_20us       << SAADC_CH_CONFIG_TACQ_Pos)   |
                            (SAADC_CH_CONFIG_MODE_SE         << SAADC_CH_CONFIG_MODE_Pos)   |
                            (SAADC_CH_CONFIG_BURST_Disabled  << SAADC_CH_CONFIG_BURST_Pos);

  NRF_SAADC->RESULT.PTR    = (uint32_t)&mb_mic_result;
  NRF_SAADC->RESULT.MAXCNT = 1U;
  NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos;
}

uint16_t microbit_mic_sample (void) {
  uint32_t timeout = 100000U;
  int16_t v;

  NRF_SAADC->EVENTS_STARTED = 0U;
  NRF_SAADC->EVENTS_END     = 0U;
  NRF_SAADC->EVENTS_STOPPED = 0U;
  NRF_SAADC->TASKS_START = 1U;
  while ((NRF_SAADC->EVENTS_STARTED == 0U) && (timeout-- != 0U)) {
  }
  NRF_SAADC->TASKS_SAMPLE = 1U;
  timeout = 100000U;
  while ((NRF_SAADC->EVENTS_END == 0U) && (timeout-- != 0U)) {
  }
  NRF_SAADC->TASKS_STOP = 1U;
  timeout = 100000U;
  while ((NRF_SAADC->EVENTS_STOPPED == 0U) && (timeout-- != 0U)) {
  }
  v = mb_mic_result;
  if (v < 0) {
    v = 0;                                  /* single-ended: small negative values at 0 V */
  }
  return (uint16_t)v;
}

uint16_t microbit_mic_level (uint32_t samples) {
  uint16_t min = 0xFFFFU;
  uint16_t max = 0U;
  uint32_t n;

  if (samples == 0U) {
    samples = 1U;
  }
  for (n = 0U; n < samples; n++) {
    uint16_t s = microbit_mic_sample();
    if (s < min) { min = s; }
    if (s > max) { max = s; }
  }
  return (uint16_t)(max - min);
}
