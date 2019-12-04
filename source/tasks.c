#include "post.h"
#include "tasks.h"
#include "circular_buffer.h"
#include "logger.h"
#include "handle_led.h"
#include "sine.h"
#include "dac_adc.h"
#include "dma.h"

//#define PROGRAM_1

struct Buffers
{
	cbuf_handle_t adcBuffer;
	cbuf_handle_t dspBuffer;
} sBuffers;

#define BUFFER_CAPACITY 64


void turn_off_dma_led(void *pvParameters)
{
	set_led(0, BLUE);
}


void DMA_Callback(dma_handle_t *handle, void *param)
{
	// copies buffer state, not data
	circular_buf_copy(sBuffers.adcBuffer, sBuffers.dspBuffer);
	circular_buf_reset(sBuffers.adcBuffer);
	LOG_STRING(LOG_MODULE_MAIN, LOG_SEVERITY_STATUS, "DMA Transfer completed.");
}

void tasks_init()
{
    TimerHandle_t writeTimerHandle = NULL;
    TimerHandle_t readTimerHandle = NULL;


    /* Create the software timer. */
    writeTimerHandle = xTimerCreate("DAC Write Timer",          /* Text name. */
    		                     pdMS_TO_TICKS(100), /* Timer period. */
                                 pdTRUE,             /* Enable auto reload. */
                                 0,                  /* ID is not used. */
								 write_dac0_task);   /* The callback function. */

    /* Create the software timer. */
    readTimerHandle = xTimerCreate("ADC READ Timer",          /* Text name. */
    		                     pdMS_TO_TICKS(100), /* Timer period. */
                                 pdTRUE,             /* Enable auto reload. */
                                 0,                  /* ID is not used. */
								 read_adc0_task);   /* The callback function. */
    /* Start timer. */
    xTimerStart(writeTimerHandle, 0);
    xTimerStart(readTimerHandle, 0);

    sBuffers.adcBuffer = circular_buf_init(BUFFER_CAPACITY);
    sBuffers.dspBuffer = circular_buf_init(BUFFER_CAPACITY);

    // need to init post-scheduler start
    //xTaskCreate(dma_init, "DMA Init", configMINIMAL_STACK_SIZE + 10, NULL, (configMAX_PRIORITIES - 1), NULL);

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
	LOG_STRING_ARGS(LOG_MODULE_MAIN, LOG_SEVERITY_DEBUG, "Writing %d to the DAC.", sineVal);
    /**
     * apply the values from the lookup table to DAC0_OUT (pin J10-11) every .1 second,
     * repeating from the beginning of the table once the last value is applied.
     * Toggle a Blue LED on and off for each visit to the timer callback.
     */
	write_dac(sineVal);

#ifdef PROGRAM_1
	set_led(ledVal, BLUE);
#else
	// todo : add synchronization primitives here
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
	LOG_STRING_ARGS(LOG_MODULE_MAIN, LOG_SEVERITY_DEBUG, "Reading %d from the ADC.", sample);
	if(circular_buf_push(sBuffers.adcBuffer, sample) == buff_err_full)
	{
		 // When the buffer is full, initiate a DMA transfer from the ADC buffer to a second
		 // buffer (called the DSP buffer).

		/*
		 When the DMA Transfer is about to start, toggle the LED to Blue for .5 seconds.
		 During this period, the LED cannot be used by other tasks.

		 Capture a time stamp at the start and completion of the DMA transfer.
		 */
		set_led(1, BLUE);
	    TimerHandle_t startTransferTimer = NULL;
	    startTransferTimer = xTimerCreate("Start DMA timer",          /* Text name. */
	    		                          pdMS_TO_TICKS(500), /* Timer period. */
	                                      pdFALSE,             /* Enable auto reload. */
	                                      0,                  /* ID is not used. */
										  turn_off_dma_led);   /* The callback function. */
	    xTimerStart(startTransferTimer, 0);

	    LOG_STRING(LOG_MODULE_MAIN, LOG_SEVERITY_STATUS, "DMA Transfer started.");
	    dma_transfer(sBuffers.adcBuffer->buffer,
	    		     sBuffers.dspBuffer->buffer,
					 BUFFER_CAPACITY);

		// copies buffer state, not data
		circular_buf_copy(sBuffers.adcBuffer, sBuffers.dspBuffer);
		circular_buf_reset(sBuffers.adcBuffer);
		LOG_STRING(LOG_MODULE_MAIN, LOG_SEVERITY_STATUS, "DMA Transfer completed.");
	}

	/*
	 Clear (or overwrite) the ADC buffer with incoming DAC values and continue sampling
	 until the next series of samples are collected.

	 You will need to consider the size and data width requirements for the ADC
	 buffer and the DSP buffer.
	 */
}
