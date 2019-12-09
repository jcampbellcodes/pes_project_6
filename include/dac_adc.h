/*
 * @file dac_adc.h
 * @brief Project 6
 *
 * @details This file contains code for using the ADC and DAC.
 *
 * @author Jack Campbell
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 *
 */

#ifndef __dach__
#define __dach__

#include <stdint.h>

/**
 * Init the DAC.
 */
void dac_init();

/**
 * Init the adc.
 */
void adc_init();

/**
 * Write a value to the DAC.
 */
void write_dac(uint32_t inVal);

/**
 * Read a value from the ADC.
 */
uint32_t read_adc();

#endif
