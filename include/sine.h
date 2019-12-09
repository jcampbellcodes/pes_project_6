/*
 * @file sine.h
 * @brief Project 6
 *
 * @details Contains the setup and cleanup prototypes.
 *
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 */

#ifndef __sinelookup__
#define __sinelookup__

#include <stdint.h>

/**
 * Initialize the sine wave lookup table.
 */
void sine_init();

// get next sine sample

/**
 * Get the next sine sample in the lookup table.
 */
uint32_t get_next_sine_sample();

#endif
