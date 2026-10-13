/*
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * BBC micro:bit V2: UARTE0 to the DAPLink interface MCU (USB serial)
 * TX P0.06, RX P1.08. Transmit is polled (one byte EasyDMA transfer),
 * receive is interrupt driven into a ring buffer.
 */
#include "microbit_v2.h"

#define MB_UART_RX_SIZE   64U

static uint8_t          mb_uart_txbyte;                 /* EasyDMA needs RAM */
static uint8_t          mb_uart_rxbyte;
static volatile uint8_t mb_uart_rxbuf[MB_UART_RX_SIZE];
static volatile uint32_t mb_uart_rx_head;
static volatile uint32_t mb_uart_rx_tail;

static uint32_t mb_uart_baud_reg (uint32_t baudrate) {
  switch (baudrate) {
    case 1200:    return UARTE_BAUDRATE_BAUDRATE_Baud1200;
    case 2400:    return UARTE_BAUDRATE_BAUDRATE_Baud2400;
    case 4800:    return UARTE_BAUDRATE_BAUDRATE_Baud4800;
    case 9600:    return UARTE_BAUDRATE_BAUDRATE_Baud9600;
    case 14400:   return UARTE_BAUDRATE_BAUDRATE_Baud14400;
    case 19200:   return UARTE_BAUDRATE_BAUDRATE_Baud19200;
    case 28800:   return UARTE_BAUDRATE_BAUDRATE_Baud28800;
    case 31250:   return UARTE_BAUDRATE_BAUDRATE_Baud31250;
    case 38400:   return UARTE_BAUDRATE_BAUDRATE_Baud38400;
    case 56000:   return UARTE_BAUDRATE_BAUDRATE_Baud56000;
    case 57600:   return UARTE_BAUDRATE_BAUDRATE_Baud57600;
    case 76800:   return UARTE_BAUDRATE_BAUDRATE_Baud76800;
    case 230400:  return UARTE_BAUDRATE_BAUDRATE_Baud230400;
    case 250000:  return UARTE_BAUDRATE_BAUDRATE_Baud250000;
    case 460800:  return UARTE_BAUDRATE_BAUDRATE_Baud460800;
    case 921600:  return UARTE_BAUDRATE_BAUDRATE_Baud921600;
    case 1000000: return UARTE_BAUDRATE_BAUDRATE_Baud1M;
    default:      return UARTE_BAUDRATE_BAUDRATE_Baud115200;
  }
}

void microbit_uart_init (uint32_t baudrate) {
  NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;

  mb_gpio_output(MB_PIN_UART_TX, 1U, false);
  mb_gpio_input (MB_PIN_UART_RX, GPIO_PIN_CNF_PULL_Disabled);

  NRF_UARTE0->PSEL.TXD = MB_PIN_UART_TX;
  NRF_UARTE0->PSEL.RXD = MB_PIN_UART_RX;
  NRF_UARTE0->PSEL.RTS = UARTE_PSEL_RTS_CONNECT_Disconnected << UARTE_PSEL_RTS_CONNECT_Pos;
  NRF_UARTE0->PSEL.CTS = UARTE_PSEL_CTS_CONNECT_Disconnected << UARTE_PSEL_CTS_CONNECT_Pos;
  NRF_UARTE0->BAUDRATE = mb_uart_baud_reg(baudrate);
  NRF_UARTE0->CONFIG   = (UARTE_CONFIG_HWFC_Disabled   << UARTE_CONFIG_HWFC_Pos) |
                         (UARTE_CONFIG_PARITY_Excluded << UARTE_CONFIG_PARITY_Pos);

  mb_uart_rx_head = 0U;
  mb_uart_rx_tail = 0U;

  NRF_UARTE0->EVENTS_ENDRX = 0U;
  NRF_UARTE0->EVENTS_ENDTX = 0U;
  NRF_UARTE0->EVENTS_ERROR = 0U;
  NRF_UARTE0->ERRORSRC     = 0xFU;
  NRF_UARTE0->RXD.PTR    = (uint32_t)&mb_uart_rxbyte;
  NRF_UARTE0->RXD.MAXCNT = 1U;
  NRF_UARTE0->SHORTS     = UARTE_SHORTS_ENDRX_STARTRX_Msk;
  NRF_UARTE0->INTENSET   = UARTE_INTENSET_ENDRX_Msk;
  NRF_UARTE0->ENABLE     = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;
  NRF_UARTE0->TASKS_STARTRX = 1U;

  NVIC_SetPriority(UARTE0_UART0_IRQn, 3U);
  NVIC_EnableIRQ(UARTE0_UART0_IRQn);
}

void UARTE0_UART0_IRQHandler (void) {
  if (NRF_UARTE0->EVENTS_ENDRX != 0U) {
    uint32_t next;
    NRF_UARTE0->EVENTS_ENDRX = 0U;
    next = (mb_uart_rx_head + 1U) % MB_UART_RX_SIZE;
    if (next != mb_uart_rx_tail) {                      /* drop the byte when the buffer is full */
      mb_uart_rxbuf[mb_uart_rx_head] = mb_uart_rxbyte;
      mb_uart_rx_head = next;
    }
  }
}

void microbit_uart_putc (char c) {
  mb_uart_txbyte = (uint8_t)c;
  NRF_UARTE0->EVENTS_ENDTX = 0U;
  NRF_UARTE0->TXD.PTR    = (uint32_t)&mb_uart_txbyte;
  NRF_UARTE0->TXD.MAXCNT = 1U;
  NRF_UARTE0->TASKS_STARTTX = 1U;
  while (NRF_UARTE0->EVENTS_ENDTX == 0U) {
  }
  NRF_UARTE0->EVENTS_ENDTX = 0U;
  NRF_UARTE0->TASKS_STOPTX = 1U;
}

void microbit_uart_write (const char *s) {
  while (*s != '\0') {
    microbit_uart_putc(*s++);
  }
}

bool microbit_uart_rx_ready (void) {
  return (mb_uart_rx_head != mb_uart_rx_tail);
}

int microbit_uart_getc (void) {
  int c;
  if (mb_uart_rx_head == mb_uart_rx_tail) {
    return -1;
  }
  c = mb_uart_rxbuf[mb_uart_rx_tail];
  mb_uart_rx_tail = (mb_uart_rx_tail + 1U) % MB_UART_RX_SIZE;
  return c;
}
