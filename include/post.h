/*
 * @file post.h
 * @brief Project 5
 *
 * A power on self test.
 *
 * @author Jack Campbell
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 */

#ifndef POSTH
#define POSTH

#include <stdbool.h>

/**
 * @brief Power on self test that checks for connection with peripherals and board functions.
 * @return Whether the test succeeded.
 */
bool power_on_self_test();

#endif
