#include "sine.h"
#include "logger.h"
#include "handle_led.h"
#include <math.h>
#define NUM_SINE_SAMPLES 50
#define INV_THREE_FACTORIAL (1/6)
#define INV_FIVE_FACTORIAL (1/120)
#define INV_SEVEN_FACTORIAL (1/5040)

#define M_PI 3.14159265358979323846

// derived from voltRead = (float)(g_Adc16ConversionValue * (VREF_BRD / SE_12BIT));
// in dac adc example
const static float sDigitalConversionFactor = 1/(3.30/4096.0);

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
void sine_init()
{
	// make a local sine function with taylor series
	// try to figure out how to generate the lookup table above
	uint64_t voltage_offset = 2;
	// generate a sine wave, between 1V and 3V: sin(x) = A*sin(x)+2
	for(int x =0; x < NUM_SINE_SAMPLES; x++)
	{
		sSineLookup[x] = (sin((2.0 * M_PI * (x/(float)(NUM_SINE_SAMPLES)))) + voltage_offset) * sDigitalConversionFactor;
		if(sSineLookup[x] > 4095)
		{
			set_led(1, RED);
			LOG_STRING_ARGS(LOG_MODULE_MAIN, LOG_SEVERITY_TEST, "Value [%d] out of range. Setting to 0.", sSineLookup[x]);
			sSineLookup[x] = 0;
		}
		else
		{
			LOG_STRING_ARGS(LOG_MODULE_MAIN, LOG_SEVERITY_TEST, "Writing [%d] to the temp table.", sSineLookup[x]);
		}

	}

	LOG_STRING(LOG_MODULE_MAIN, LOG_SEVERITY_TEST, "Sine initialized.");
}

// get next sine sample
uint32_t get_next_sine_sample()
{
	static uint32_t sNextSample = 0;
	return sSineLookup[sNextSample++ % NUM_SINE_SAMPLES]; // TODO: mod is expensive!
}
