/*
 * @file handle_led.h
 * @brief Project 5
 *
 * @details Contains the prototype for handling LEDs on various platforms.
 *          This may be actually turning an LED on and off or just printing
 *          what the LED state would be, in the absence of LEDs.
 *
 * @author Jack Campbell
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 */
#ifndef PES_PROJECT_4_HANDLE_LED_H
#define PES_PROJECT_4_HANDLE_LED_H

#include <stdint.h>
#include "led_types.h"

/**
 * @brief Initializes the LED functions.
 */
void leds_init();

/**
 * set_led
 *
 * @brief Sets the LED state.
 * @details This function, depending on platform, may or may not
 *          control a physical LED. On PC, it will simply print the
 *          state of what the LED would be.
 * @param inValue The on/off state of the LED to set.
 * @param inColor The color of the LED to set.
 */
void set_led(uint8_t inValue, enum COLOR inColor);

#endif //PES_PROJECT_2_HANDLE_LED_H
