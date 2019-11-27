#ifndef __tashsh__
#define __tashsh__

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

void tasks_init();

void write_dac0_task(TimerHandle_t xTimer);

void read_adc0_task(TimerHandle_t xTimer);

void dma_transfer_complete(void *pvParameters);


#endif
