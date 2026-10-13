/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * CMSIS-Compiler STDIO retarget to the micro:bit V2 UART (USB serial of the DAPLink interface)
 */
#include "RTE_Components.h"
#include "microbit_v2.h"

#ifdef RTE_CMSIS_Compiler_STDOUT_Custom
#include "retarget_stdout.h"
int stdout_putchar (int ch) {
  if (ch == '\n') {
    microbit_uart_putc('\r');
  }
  microbit_uart_putc((char)ch);
  return ch;
}
#endif

#ifdef RTE_CMSIS_Compiler_STDERR_Custom
#include "retarget_stderr.h"
int stderr_putchar (int ch) {
  if (ch == '\n') {
    microbit_uart_putc('\r');
  }
  microbit_uart_putc((char)ch);
  return ch;
}
#endif

#ifdef RTE_CMSIS_Compiler_STDIN_Custom
#include "retarget_stdin.h"
int stdin_getchar (void) {
  int c;
  while ((c = microbit_uart_getc()) < 0) {
    __WFI();
  }
  return c;
}
#endif
