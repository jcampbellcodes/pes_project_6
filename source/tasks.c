// LEVERAGED CODE: STANDARD DEVIATION https://www.sanfoundry.com/c-program-mean-variance-standard-deviation/

#include "post.h"
#include "tasks.h"
#include "circular_buffer.h"
#include "logger.h"
#include "handle_led.h"
#include "sine.h"
#include "dac_adc.h"
#include "dma.h"
#include "time.h"
#include <float.h>
#include <math.h>

//#define PROGRAM_1

static TimerHandle_t writeTimerHandle = NULL;
static TimerHandle_t readTimerHandle = NULL;

static uint64_t sLastDMAStart = 0;
static uint64_t sLastDMAFinish = 0;

// taken from dac/adc example
#define VREF_BRD 3.300
#define SE_12BIT 4096.0

struct Buffers
{
	cbuf_handle_t adcBuffer;
	cbuf_handle_t dspBuffer;
} sBuffers;

#define BUFFER_CAPACITY 64

#define NUM_RUNS 5

void turn_off_dma_led(void *pvParameters)
{
	LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Turn off blue LED triggered by DMA transfer.");
	set_led(0, BLUE);
}

void dsp_callback(void *pvParameters)
{
	static uint8_t sRunNumber = 0;
	static float sMaxVoltage = 0;
	static float sMinVoltage = FLT_MAX;
	static float sAverageVoltage = 0;
	static float sVoltagesCumulative = 0;
	static float sNumVoltagesRecorded = 0;

	static float sVarianceSum = 0;
	static float sVariance = 0;
	static float sStDeviationVoltage = 0;

	sRunNumber++;

	float voltages[BUFFER_CAPACITY] = {0.0f};

	/**
	 * Calculate the following floating point values
	 * from the ADC register values:  maximum,  minimum, average,
	 * and standard deviation of voltage levels.
	 */

	uint32_t data;
	uint32_t i = 0;
	while(circular_buf_pop(sBuffers.dspBuffer, &data) == buff_err_success)
	{
		sNumVoltagesRecorded++;

        // convert to float
		float voltage = (float)(data * (VREF_BRD / SE_12BIT));

		// calc max
		if(voltage > sMaxVoltage)
		{
			sMaxVoltage = voltage;
		}

		// calc min
		if(voltage < sMinVoltage)
		{
			sMinVoltage = voltage;
		}
		// calc avg
		sVoltagesCumulative += voltage;
		sAverageVoltage = (sVoltagesCumulative)/sNumVoltagesRecorded;

		voltages[i] = voltage;
		i++;
	}

    /*  Compute  variance  and standard deviation  */
    for (int iter = 0; iter < BUFFER_CAPACITY; iter++)
    {
    	float voltage_entry = (voltages[iter] - sAverageVoltage);
		sVarianceSum = sVarianceSum + (voltage_entry * voltage_entry); // cheap square
    }
    sVariance = sVarianceSum / (float)sNumVoltagesRecorded;
    sStDeviationVoltage = sqrt(sVariance); //TODO remove if we can't afford this

	/**
	 * Report those values along with an incremented run number
	 * starting at 1 and the start time and end time for the last DMA transfer.
	 */
	LOG_STRING_ARGS(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Run #%d. DMA [Start : Finish] stamps: [ %lu : %lu ]:", sRunNumber, sLastDMAStart, sLastDMAFinish);

	// report max
	LOG_STRING_ARGS(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Maximum voltage: %f", sMaxVoltage);

	// report min
	LOG_STRING_ARGS(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Minimum voltage: %f", sMinVoltage);

	// report avg
	LOG_STRING_ARGS(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Average voltage: %f", sAverageVoltage);

	// report st deviation
	LOG_STRING_ARGS(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Standard deviation voltage: %f", sStDeviationVoltage);

	/**
	 * Once run number 5 is completed and reported, terminate the
	 * DAC and ADC tasks, and terminate this task to end the program.
	 */
	if(sRunNumber >= NUM_RUNS)
	{
		LOG_STRING_ARGS(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Exiting app.", sRunNumber);

		xTimerStop(writeTimerHandle, 0);
		xTimerStop(readTimerHandle, 0);
	}

	vTaskDelete(NULL);
}

void DMA_Callback()
{
	// copies buffer state, not data
	circular_buf_copy(sBuffers.adcBuffer, sBuffers.dspBuffer);
	circular_buf_reset(sBuffers.adcBuffer);
	LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_STATUS, "DMA Transfer completed.");
	sLastDMAFinish = time_now();

	if(xTaskCreate(dsp_callback, "DSP Callback", configMINIMAL_STACK_SIZE + 512, NULL, (configMAX_PRIORITIES - 1), NULL) != pdPASS)
	{
		LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_STATUS, "DSP task creation failed.");
		set_led(1, RED);
	}
}

void tasks_init()
{

    LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Create .1 second timer to write sine values to the DAC.");
    /* Create the software timer. */
    writeTimerHandle = xTimerCreate("DAC Write Timer",          /* Text name. */
    		                     pdMS_TO_TICKS(100), /* Timer period. */
                                 pdTRUE,             /* Enable auto reload. */
                                 0,                  /* ID is not used. */
								 write_dac0_task);   /* The callback function. */
    xTimerStart(writeTimerHandle, 0);

    // program 1 only cares about writing to the DAC0_OUT
#ifndef PROGRAM_1
    LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Create .1 second timer to read sine values from the ADC.");
    /* Create the software timer. */
    readTimerHandle = xTimerCreate("ADC READ Timer",          /* Text name. */
    		                     pdMS_TO_TICKS(100), /* Timer period. */
                                 pdTRUE,             /* Enable auto reload. */
                                 0,                  /* ID is not used. */
								 read_adc0_task);   /* The callback function. */
    xTimerStart(readTimerHandle, 0);

    LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Create DSP and ADC buffers.");
    sBuffers.adcBuffer = circular_buf_init(BUFFER_CAPACITY);
    sBuffers.dspBuffer = circular_buf_init(BUFFER_CAPACITY);
    // need to init post-scheduler start
    //xTaskCreate(dma_init, "DMA Init", configMINIMAL_STACK_SIZE + 512, NULL, (configMAX_PRIORITIES - 1), NULL);
#endif
    /* Start scheduling. */
    vTaskStartScheduler();
}


/*!
 * @brief Software timer callback.
 */
void write_dac0_task(TimerHandle_t xTimer)
{
	static int ledVal = 0;
	uint32_t sineVal = get_next_sine_sample();
	LOG_STRING_ARGS(LOG_MODULE_TASKS, LOG_SEVERITY_TEST, "Writing %d to the DAC.", sineVal);
    /**
     * apply the values from the lookup table to DAC0_OUT (pin J10-11) every .1 second,
     * repeating from the beginning of the table once the last value is applied.
     * Toggle a Blue LED on and off for each visit to the timer callback.
     */
	write_dac(sineVal);

#ifdef PROGRAM_1
	LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_TEST, "Toggle blue led for program 1 DAC write.");
	set_led(ledVal, BLUE);
#else
	// todo : add synchronization primitives here
	LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_TEST, "Toggle green led for program 2 DAC write.");
	set_led(ledVal, GREEN);
#endif
	ledVal = !ledVal;
}

void read_adc0_task(TimerHandle_t xTimer)
{

	/*
	 Create a FreeRTOS Task to periodically (every .1 seconds)
	 read the DAC0 value via ADC0 and store it in a circular buffer.

	 The ADC buffer will be 64 samples long and should contain the
	 raw ADC register values from each read.

    */

	uint32_t sample = read_adc();
	LOG_STRING_ARGS(LOG_MODULE_TASKS, LOG_SEVERITY_TEST, "Reading %d from the ADC.", sample);
	if(circular_buf_push(sBuffers.adcBuffer, sample) == buff_err_full)
	{
		 // When the buffer is full, initiate a DMA transfer from the ADC buffer to a second
		 // buffer (called the DSP buffer).
		/*
		 When the DMA Transfer is about to start, toggle the LED to Blue for .5 seconds.
		 During this period, the LED cannot be used by other tasks.

		 Capture a time stamp at the start and completion of the DMA transfer.
		 */
		LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_DEBUG, "Turn on blue LED triggered by DMA transfer.");
		set_led(1, BLUE);
	    TimerHandle_t startTransferTimer = NULL;
	    startTransferTimer = xTimerCreate("Start DMA timer",          /* Text name. */
	    		                          pdMS_TO_TICKS(500), /* Timer period. */
	                                      pdFALSE,             /* Enable auto reload. */
	                                      0,                  /* ID is not used. */
										  turn_off_dma_led);   /* The callback function. */
	    xTimerStart(startTransferTimer, 0);

	    sLastDMAStart = time_now();
	    LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_STATUS, "DMA Transfer started.");
	    dma_transfer(sBuffers.adcBuffer->buffer,
	    		     sBuffers.dspBuffer->buffer,
					 BUFFER_CAPACITY,
					 DMA_Callback);

	    LOG_STRING(LOG_MODULE_TASKS, LOG_SEVERITY_TEST, "DMA Transfer claimed to complete.");
	}

	/*
	 Clear (or overwrite) the ADC buffer with incoming DAC values and continue sampling
	 until the next series of samples are collected.

	 You will need to consider the size and data width requirements for the ADC
	 buffer and the DSP buffer.
	 */
}
