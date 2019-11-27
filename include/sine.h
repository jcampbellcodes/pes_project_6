#ifndef __sinelookup__
#define __sinelookup__

#include <stdint.h>

// populate lookup
void init_sine();

// get next sine sample
uint32_t get_next_sine_sample();

#endif
