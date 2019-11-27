/*
 * @file led_types.h
 * @brief Project 5
 *
 * @details Defines enumerations and constants used to describe colors and
 *          on/off states for LEDs.
 *
 * @author Jack Campbell
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 */

#ifndef PES_PROJECT_2_LED_TYPES_H
#define PES_PROJECT_2_LED_TYPES_H

/**
 * COLOR
 *
 * @brief The possible color values of the LED.
 */
enum COLOR
{
    RED = 0,
    GREEN,
    BLUE,
    NUM_COLORS
};

/**
 * COLOR_STRINGS
 *
 * @brief String representations of the COLOR enum, used for printing.
 */
static const char * const COLOR_STRINGS[3] = {
        "RED",
        "GREEN",
        "BLUE"
};

#endif //PES_PROJECT_2_LED_TYPES_H
