#include "sine.h"
#include "logger.h"
#include <math.h>
#define NUM_SINE_SAMPLES 50
#define INV_THREE_FACTORIAL (1/6)
#define INV_FIVE_FACTORIAL (1/120)
#define INV_SEVEN_FACTORIAL (1/5040)

#define M_PI 3.14159265358979323846

// lookup table
// TODO Generate this table..
static uint32_t sSineLookup[NUM_SINE_SAMPLES] = {0};


float sinef(float x)
{
	// LEVERAGED FROM WHITE BOOK
	float xSq = x * x;

	return x * (1 - xSq * (INV_THREE_FACTORIAL + xSq *
	           (INV_FIVE_FACTORIAL - INV_SEVEN_FACTORIAL * xSq)));
}

// populate lookup
void init_sine()
{
	// make a local sine function with taylor series
	// try to figure out how to generate the lookup table above
	uint64_t voltage_offset = 2;
	// generate a sine wave, between 1V and 3V: sin(x) = A*sin(x)+2
	for(int x =0; x < NUM_SINE_SAMPLES; x++)
	{
		// short conversion here https://community.nxp.com/thread/440048
		sSineLookup[x] = (uint16_t)(((sin((2.0 * M_PI * x) / (float)(NUM_SINE_SAMPLES)) + voltage_offset) * 0x7ff) + (0xfff/2));
		LOG_STRING_ARGS(LOG_MODULE_MAIN, LOG_SEVERITY_TEST, "Writing [%d] to the temp table.", sSineLookup[x]);
	}
}

// get next sine sample
uint32_t get_next_sine_sample()
{
	static uint32_t sNextSample = 0;
	return sSineLookup[sNextSample++ % NUM_SINE_SAMPLES]; // TODO: mod is expensive!
}
