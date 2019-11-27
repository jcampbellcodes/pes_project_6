

/* Standard includes. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"

#include "setup_teardown.h"
#include "dac.h"
#include "sine.h"
#include "logger.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define ever (;;);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
	initialize();

    for ever
}

