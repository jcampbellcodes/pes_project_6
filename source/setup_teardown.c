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

//TODO: trim down includes
#include "board.h"
#include "peripherals.h"
#include "clock_config.h"
#include "pin_mux.h"


/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "tasks.h"
#include "post.h"
#include "circular_buffer.h"
#include "logger.h"
#include <stdlib.h>
#include "handle_led.h"
#include "time.h"
#include "sine.h"
#include "dac_adc.h"
#include "MKL25Z4.h"
#include "uart.h"

void initialize()
{
    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    SystemCoreClockUpdate();


  	/* Init board hardware. */
	 /* Enable all of the port clocks. */
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK
			   | SIM_SCGC5_PORTB_MASK
			   | SIM_SCGC5_PORTC_MASK
			   | SIM_SCGC5_PORTD_MASK
			   | SIM_SCGC5_PORTE_MASK );

#ifdef DEBUG
	log_enable(LOG_SEVERITY_DEBUG);
#else
	log_enable(LOG_SEVERITY_STATUS);
#endif
	uart_init(115200); // todo define this
	time_init();
    leds_init();
    sine_init();
    dac_init();
    adc_init();
    power_on_self_test();

    tasks_init();
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
