/*
 * @file time.c
 * @brief Project 5
 *
 * @details Contains interface for telling and initializing time.
 *
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 *
 *  LEVERAGED CODE: This time-passed and time_now pseudocode
 *  is taken from the White book, p140.
 */

#include "MKL25Z4.h"
#include <stdbool.h>
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "time.h"
#include <stdio.h>

/* The software timer period. */
#define SW_TIMER_PERIOD_MS ( 10 / portTICK_PERIOD_MS )

// tenths of a second
static uint64_t gSystemTime = 0;

void time_ticker()
{
	gSystemTime++;
}


void time_init()
{
    TimerHandle_t SwTimerHandle = NULL;

    /* Create the software timer. */
    SwTimerHandle = xTimerCreate("TimeTicker",          /* Text name. */
    		                     pdMS_TO_TICKS(100), /* Timer period. */
                                 pdTRUE,             /* Enable auto reload. */
                                 0,                  /* ID is not used. */
								 time_ticker);   /* The callback function. */
    /* Start timer. */
    xTimerStart(SwTimerHandle, 0);
}


// taken from the white book
uint64_t time_passed(uint64_t since)
{
	// used to rollover the time
	static const uint64_t gTimeMax = ~0;
	uint64_t now = gSystemTime;

	if(now >= since)
	{
		return now - since;
	}

	// rollover has occurred
	return (now + (gTimeMax-since));
}

uint64_t time_now()
{
	return gSystemTime;
}

void timestamp_now(timestamp_str* outTimestamp)
{
	uint64_t tenths_seconds = time_now();
	float now = tenths_seconds / 10;
	uint64_t hours = (uint64_t)(now/3600)%60;
	uint64_t minutes = (uint64_t)(now/60)%60;
	uint64_t seconds = (uint64_t)(now)%60;
	sprintf((outTimestamp->hours), "%02d:",  hours);
	sprintf((outTimestamp->mins),  "%02d:",  minutes);
	sprintf((outTimestamp->secs),  "%02d",  seconds);
	sprintf((outTimestamp->tens),  ".%1d ",  tenths_seconds%10);
}

