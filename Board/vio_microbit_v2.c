/******************************************************************************
 * @file     vio_microbit_v2.c
 * @brief    Virtual I/O implementation for the BBC micro:bit V2
 * @version  V1.0.0
 * @date     9. October 2026
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

/*! \page vio_microbit_v2 Physical I/O Mapping

The table below lists the physical I/O mapping of this CMSIS-Driver VIO implementation.

| Virtual I/O   | Variable       | Board component            | Pin
|:--------------|:---------------|:---------------------------|:---------------------
| vioBUTTON0    | vioSignalIn.0  | Button A                   | P0.14 (active low)
| vioBUTTON1    | vioSignalIn.1  | Button B                   | P0.23 (active low)
| vioLED0       | vioSignalOut.0 | LED matrix row 1, column 1 | ROW1 P0.21 / COL1 P0.28
| vioLED1       | vioSignalOut.1 | LED matrix row 1, column 2 | ROW1 P0.21 / COL2 P0.11
| vioLED2       | vioSignalOut.2 | LED matrix row 1, column 3 | ROW1 P0.21 / COL3 P0.31
| vioLED3       | vioSignalOut.3 | LED matrix row 1, column 4 | ROW1 P0.21 / COL4 P1.05
| vioLED4       | vioSignalOut.4 | LED matrix row 1, column 5 | ROW1 P0.21 / COL5 P0.30

The 5x5 LED matrix is row/column multiplexed. This driver uses only the top row
(ROW1 driven high permanently) so that the five LEDs of that row can be controlled
statically without a refresh timer: an LED is on when its column is driven low.
Rows 2..5 are driven low, which keeps their LEDs off.
*/

#include <string.h>
#include "cmsis_vio.h"

#include "RTE_Components.h"                 // Component selection
#include CMSIS_device_header

// VIO input, output definitions
#ifndef VIO_VALUE_NUM
#define VIO_VALUE_NUM           5U          // Number of values
#endif

// VIO input, output variables
__USED uint32_t vioSignalIn;                // Memory for incoming signal
__USED uint32_t vioSignalOut;               // Memory for outgoing signal
__USED int32_t  vioValue[VIO_VALUE_NUM];    // Memory for value used in vioGetValue/vioSetValue

// GPIO pin descriptor
typedef struct {
  NRF_GPIO_Type *port;
  uint32_t       pin;
} vioPin_t;

// LED matrix row pins (ROW1..ROW5): LED on = row high and column low
static const vioPin_t rowPins[5] = {
  { NRF_P0, 21U },                          // ROW1
  { NRF_P0, 22U },                          // ROW2
  { NRF_P0, 15U },                          // ROW3
  { NRF_P0, 24U },                          // ROW4
  { NRF_P0, 19U }                           // ROW5
};

// LED matrix column pins (COL1..COL5) mapped to vioLED0..vioLED4
static const vioPin_t colPins[5] = {
  { NRF_P0, 28U },                          // COL1 -> vioLED0
  { NRF_P0, 11U },                          // COL2 -> vioLED1
  { NRF_P0, 31U },                          // COL3 -> vioLED2
  { NRF_P1,  5U },                          // COL4 -> vioLED3
  { NRF_P0, 30U }                           // COL5 -> vioLED4
};

// Button pins (active low, external pull-ups on the board) mapped to vioBUTTON0..vioBUTTON1
static const vioPin_t btnPins[2] = {
  { NRF_P0, 14U },                          // Button A -> vioBUTTON0
  { NRF_P0, 23U }                           // Button B -> vioBUTTON1
};

#define VIO_LED_NUM     (sizeof(colPins) / sizeof(colPins[0]))
#define VIO_ROW_NUM     (sizeof(rowPins) / sizeof(rowPins[0]))
#define VIO_BUTTON_NUM  (sizeof(btnPins) / sizeof(btnPins[0]))

// Configure a pin as push-pull output with the given initial level
static void pinOutput (const vioPin_t *p, uint32_t level) {
  if (level != 0U) {
    p->port->OUTSET = (1UL << p->pin);
  } else {
    p->port->OUTCLR = (1UL << p->pin);
  }
  p->port->PIN_CNF[p->pin] = (GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)   |
                             (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
                             (GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)  |
                             (GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos) |
                             (GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
}

// Configure a pin as input with pull-up
static void pinInput (const vioPin_t *p) {
  p->port->PIN_CNF[p->pin] = (GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)   |
                             (GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos) |
                             (GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)  |
                             (GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos) |
                             (GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
}

// Initialize test input, output.
void vioInit (void) {
  uint32_t n;

  vioSignalIn  = 0U;
  vioSignalOut = 0U;

  memset(vioValue, 0, sizeof(vioValue));

  // LED matrix: all columns high (LEDs off), row 1 high, rows 2..5 low
  for (n = 0U; n < VIO_LED_NUM; n++) {
    pinOutput(&colPins[n], 1U);
  }
  for (n = 0U; n < VIO_ROW_NUM; n++) {
    pinOutput(&rowPins[n], (n == 0U) ? 1U : 0U);
  }

  // Buttons
  for (n = 0U; n < VIO_BUTTON_NUM; n++) {
    pinInput(&btnPins[n]);
  }
}

// Set signal output.
void vioSetSignal (uint32_t mask, uint32_t signal) {
  uint32_t n;

  vioSignalOut &= ~mask;
  vioSignalOut |=  mask & signal;

  for (n = 0U; n < VIO_LED_NUM; n++) {
    if ((mask & (1UL << n)) != 0U) {
      // LED on: column low; LED off: column high
      if ((signal & (1UL << n)) != 0U) {
        colPins[n].port->OUTCLR = (1UL << colPins[n].pin);
      } else {
        colPins[n].port->OUTSET = (1UL << colPins[n].pin);
      }
    }
  }
}

// Get signal input.
uint32_t vioGetSignal (uint32_t mask) {
  uint32_t signal;
  uint32_t n;

  for (n = 0U; n < VIO_BUTTON_NUM; n++) {
    if ((mask & (1UL << n)) != 0U) {
      // Button pressed = pin low
      if ((btnPins[n].port->IN & (1UL << btnPins[n].pin)) == 0U) {
        vioSignalIn |=  (1UL << n);
      } else {
        vioSignalIn &= ~(1UL << n);
      }
    }
  }

  signal = vioSignalIn & mask;

  return signal;
}

// Set value output.
void vioSetValue (uint32_t id, int32_t value) {
  uint32_t index = id;

  if (index >= VIO_VALUE_NUM) {
    return;                             /* return in case of out-of-range index */
  }

  vioValue[index] = value;
}

// Get value input.
int32_t vioGetValue (uint32_t id) {
  uint32_t index = id;
  int32_t  value = 0;

  if (index >= VIO_VALUE_NUM) {
    return value;                       /* return default in case of out-of-range index */
  }

  value = vioValue[index];

  return value;
}
