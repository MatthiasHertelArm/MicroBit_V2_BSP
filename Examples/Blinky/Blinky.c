/*---------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------*/

#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_vio.h"

static volatile uint32_t msTicks;           /* 1 ms tick counter */

/* SysTick interrupt: 1 ms period */
void SysTick_Handler (void) {
  msTicks++;
}

/* Wait for the given number of milliseconds */
static void delay_ms (uint32_t ms) {
  uint32_t start = msTicks;

  while ((msTicks - start) < ms) {
    __WFI();
  }
}

int main (void) {
  uint32_t led0 = 0U;
  uint32_t n    = 0U;

  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000U);  /* 1 ms SysTick */

  vioInit();

  for (;;) {
    /* vioLED0 blinks with 1 Hz (toggled every 500 ms) */
    if (++n >= 10U) {
      n = 0U;
      led0 ^= 1U;
      vioSetSignal(vioLED0, (led0 != 0U) ? vioLEDon : vioLEDoff);
    }

    /* vioLED1 follows button A, vioLED2 follows button B */
    vioSetSignal(vioLED1, (vioGetSignal(vioBUTTON0) != 0U) ? vioLEDon : vioLEDoff);
    vioSetSignal(vioLED2, (vioGetSignal(vioBUTTON1) != 0U) ? vioLEDon : vioLEDoff);

    delay_ms(50U);
  }
}
