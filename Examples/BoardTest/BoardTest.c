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

/*
 * BoardTest: exercises every on-board peripheral of the BBC micro:bit V2
 *
 *  - UART (USB serial, 115200 8N1): status report every second, single-key commands
 *  - LED matrix: 'A'/'B' while a button is pressed, heart while the logo is touched,
 *    otherwise a dot that follows the tilt of the board (accelerometer)
 *  - Speaker: 440 Hz while the logo is touched; command 't' plays a short tone
 *  - Microphone: peak-to-peak level; a loud sound shows '!' for half a second
 *  - LSM303AGR accelerometer and magnetometer, nRF52833 temperature sensor
 */

#include <stdio.h>
#include "RTE_Components.h"
#include CMSIS_device_header
#include "microbit_v2.h"

static const uint8_t img_heart[5] = { 0x0A, 0x1F, 0x1F, 0x0E, 0x04 };
static const uint8_t img_full[5]  = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };

static int  motion_ok;
static uint32_t loud_until;
static uint32_t tone_until;

static void print_help (void) {
  printf("Commands: r report, t tone (880 Hz, 300 ms), d display test, m toggle microphone power, h help\n");
}

static void print_report (void) {
  microbit_vec3_t acc = {0, 0, 0}, mag = {0, 0, 0};
  int32_t t = microbit_temperature_read();
  uint16_t mic = microbit_mic_level(200U);
  uint32_t logo = microbit_touch_logo_raw();

  if (motion_ok == 0) {
    (void)microbit_accel_read(&acc);
    (void)microbit_mag_read(&mag);
  }
  printf("t=%lu ms  temp=%ld.%02ld C  acc=(%d,%d,%d) mg  mag=(%d,%d,%d) mG  mic_pp=%u  btnA=%d btnB=%d  logo=%lu/%lu\n",
         (unsigned long)microbit_time_ms(),
         (long)(t / 4), (long)((t % 4) * 25),
         acc.x, acc.y, acc.z, mag.x, mag.y, mag.z,
         (unsigned)mic,
         microbit_button_pressed(MICROBIT_BUTTON_A) ? 1 : 0,
         microbit_button_pressed(MICROBIT_BUTTON_B) ? 1 : 0,
         (unsigned long)logo, (unsigned long)microbit_touch_logo_baseline());
}

static void handle_command (int c) {
  static bool mic_on = true;

  switch (c) {
    case 'r':
      print_report();
      break;
    case 't':
      microbit_speaker_tone(880U);
      tone_until = microbit_time_ms() + 300U;
      printf("tone 880 Hz\n");
      break;
    case 'd':
      microbit_display_show_image(img_full);
      microbit_delay_ms(500U);
      microbit_display_clear();
      printf("display test done\n");
      break;
    case 'm':
      mic_on = !mic_on;
      microbit_mic_power(mic_on);
      printf("microphone %s\n", mic_on ? "on" : "off");
      break;
    case 'h':
    case '?':
      print_help();
      break;
    default:
      break;
  }
}

int main (void) {
  uint32_t last_report = 0U;
  uint32_t mic_base = 0U;
  char ch;

  microbit_time_init();
  microbit_uart_init(115200U);

  printf("\n\nBBC micro:bit V2 BoardTest (nRF52833, SystemCoreClock %lu Hz)\n", (unsigned long)SystemCoreClock);

  microbit_display_init();
  microbit_button_init();
  microbit_speaker_init();
  microbit_mic_init();
  microbit_i2c_init();
  motion_ok = microbit_motion_init();
  printf("LSM303AGR: %s\n", (motion_ok == 0) ? "found (WHO_AM_I 0x33 / 0x40)" : "NOT FOUND");

  microbit_touch_init();
  printf("Touch logo baseline: %lu\n", (unsigned long)microbit_touch_logo_baseline());

  /* show the characters H I on the matrix, then the test pattern */
  for (ch = 'H'; ch <= 'I'; ch++) {
    microbit_display_show_char(ch);
    microbit_delay_ms(400U);
  }
  microbit_display_show_image(img_full);
  microbit_delay_ms(300U);
  microbit_display_clear();

  mic_base = microbit_mic_level(200U);
  printf("Microphone idle level (peak-to-peak): %lu\n", (unsigned long)mic_base);
  print_help();
  print_report();

  for (;;) {
    uint32_t now = microbit_time_ms();
    bool a = microbit_button_pressed(MICROBIT_BUTTON_A);
    bool b = microbit_button_pressed(MICROBIT_BUTTON_B);
    bool logo = microbit_touch_logo_pressed();
    int  c;

    /* serial commands */
    while ((c = microbit_uart_getc()) >= 0) {
      handle_command(c);
    }

    /* microphone: loud sound detector */
    if (microbit_mic_level(100U) > (mic_base * 3U + 200U)) {
      loud_until = now + 500U;
    }

    /* speaker */
    if (logo) {
      microbit_speaker_tone(440U);
    } else if ((tone_until != 0U) && ((int32_t)(now - tone_until) >= 0)) {
      microbit_speaker_off();
      tone_until = 0U;
    } else if (tone_until == 0U) {
      microbit_speaker_off();
    }

    /* display */
    if (a && b) {
      microbit_display_show_char('+');
    } else if (a) {
      microbit_display_show_char('A');
    } else if (b) {
      microbit_display_show_char('B');
    } else if (logo) {
      microbit_display_show_image(img_heart);
    } else if ((int32_t)(now - loud_until) < 0) {
      microbit_display_show_char('!');
    } else {
      /* dot follows the tilt: x right, y towards the edge connector */
      microbit_vec3_t acc = {0, 0, 0};
      int32_t x = 2, y = 2;
      if ((motion_ok == 0) && (microbit_accel_read(&acc) == 0)) {
        x = 2 + (acc.x / 250);
        y = 2 + (acc.y / 250);
        if (x < 0) { x = 0; } else if (x > 4) { x = 4; }
        if (y < 0) { y = 0; } else if (y > 4) { y = 4; }
      }
      microbit_display_clear();
      microbit_display_set_pixel((uint32_t)x, (uint32_t)y, true);
    }

    /* report */
    if ((now - last_report) >= 1000U) {
      last_report = now;
      print_report();
    }

    microbit_delay_ms(20U);
  }
}
