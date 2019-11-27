/*
 * @file setup_teardown.h
 * @brief Project 5
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

#include "board.h"
#include "peripherals.h"
#include "clock_config.h"
#include "pin_mux.h"


/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "post.h"
#include "logger.h"
#include <stdlib.h>
#include "handle_led.h"
#include "time.h"
#include "sine.h"
#include "dac_adc.h"
#include "MKL25Z4.h"
#include "uart.h"

//#define PROGRAM_1

/* The callback function. */
static void write_dac0_task(TimerHandle_t xTimer);

static void read_adc0_task(TimerHandle_t xTimer);

static void dma_transfer_complete(void *pvParameters);

void initialize()
{
    TimerHandle_t writeTimerHandle = NULL;
    TimerHandle_t readTimerHandle = NULL;

    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    SystemCoreClockUpdate();
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

  	/* Init board hardware. */
	 /* Enable all of the port clocks. */
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK
			   | SIM_SCGC5_PORTB_MASK
			   | SIM_SCGC5_PORTC_MASK
			   | SIM_SCGC5_PORTD_MASK
			   | SIM_SCGC5_PORTE_MASK );

#ifdef DEBUG
	log_enable(LOG_SEVERITY_TEST);
#else
	log_enable(LOG_SEVERITY_STATUS);
#endif
	uart_init(115200); // todo define this
	time_init();
    leds_init();
    sine_init();
    dac_init();

    // TODO: calculate the actul stack sizes for these tasks
    xTaskCreate(power_on_self_test, "POST", configMINIMAL_STACK_SIZE + 256, NULL, (configMAX_PRIORITIES - 1), NULL);
    xTaskCreate(dma_transfer_complete, "DMA Transfer Complete", configMINIMAL_STACK_SIZE + 256, NULL, tskIDLE_PRIORITY + 2, NULL);

    /* Start scheduling. */
    vTaskStartScheduler();
}

/*!
 * @brief Software timer callback.
 */
static void write_dac0_task(TimerHandle_t xTimer)
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

static void read_adc0_task(TimerHandle_t xTimer)
{
}

static void dma_transfer_complete(void *pvParameters)
{
}

/**
 * terminate
 *
 * @details Print "program end" in debug builds.
 *          Shows that the program successfully completed.
 *
 */
void terminate()
{
#ifdef DEBUG
	LOG_STRING(LOG_MODULE_SETUP_TEARDOWN, LOG_SEVERITY_DEBUG, "program end");
#endif
}
