/*
 * @file uart.h
 * @brief Project 5
 *
 * @details Contains interface for UART communications.
 *
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 *  LEVERAGED CODE:
 *  https://github.com/alexander-g-dean/ESF/tree/master/Code/Chapter_8/Serial-Demo
 */

#ifndef __uart_H__
#define __uart_H__

#include "MKL25Z4.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Whether to use polling or interrupts for UART communication
 */
#define USE_UART_INTERRUPTS 	(0) // 0 for polled UART communications, 1 for interrupt-driven

/**
 * @brief How much to oversample the uart clock
 */
#define UART_OVERSAMPLE_RATE 	(16)

/**
 * @brief A define for 48000000 clock rate
 */
#define SYS_CLOCK							(48e6)

/**
 * @brief Initialize the uart module
 * @param baud_rate Bits per second to use.
 */
void uart_init(int64_t baud_rate);

/**
 * @brief Poll to get a character
 * @param outChar The character received
 * @return Whether there was a character to get.
 */
bool uart_getchar(uint8_t* outChar);

/**
 * @brief Transmit a character over UART
 * @param ch Character to transmit
 */
void uart_putchar(char ch);

/**
 * @brief Get whether we have space to transmit a character
 */
bool uart_putchar_space_available();

/**
 * @brief Get whether a character has been received
 */
bool uart_getchar_present();

/**
 * @brief Send an entire string over UART
 * @param inChar String to send
 */
void uart_put_string(const char* inChar);

/**
 * @brief Respond to received characters by transmitting them back.
 * @param outChar Output parameter for the character received
 * @return Whether we echo'd.
 */
bool uart_echo(uint8_t* outChar);

#endif
