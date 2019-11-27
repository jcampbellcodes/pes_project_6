#ifndef __dach__
#define __dach__

#include <stdint.h>

void dac_init();

void adc_init();

void write_dac(uint32_t inVal);

uint32_t read_adc();

#endif
