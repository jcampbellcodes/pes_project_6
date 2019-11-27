/*
 * @file time.h
 * @brief Project 5
 *
 * @details Contains interface for telling and initializing time.
 *
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 */

#ifndef __timeh__
#define __timeh__

#include <stdint.h>

/**
 * @brief Initialize the time module
 */
void time_init();

/**
 * @brief How much time has passed, in tenths of a second
 * @param since Base time to calculate current difference with
 * @return
 */
uint64_t time_passed(uint64_t since);

/**
 * @brief Return current time in tenth of a second
 */
uint64_t time_now();

#endif
