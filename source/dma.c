#include "dma.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "MKL25Z4.h"
#include "logger.h"

static dma_callback sCurrentCallback = NULL;
volatile bool g_DMADoneFlag = false;
void DMA0_DriverIRQHandler(void)
{
	g_DMADoneFlag = true;
	// mark end of transfer
	// Control_RGB_LEDs(0,0,0); TODO: Turn off leds here?
	if(sCurrentCallback)
		sCurrentCallback();
}

// taken from https://github.com/alexander-g-dean/ESF/blob/master/Code/Chapter_9/DMA_Examples/Source/DMA.c
void dma_init(void* cookie)
{
	LOG_STRING(LOG_MODULE_DMA, LOG_SEVERITY_STATUS, "Initialize DMA.");
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	DMA0->DMA[0].DCR = DMA_DCR_SINC_MASK | DMA_DCR_SSIZE(0) |
		DMA_DCR_DINC_MASK |	DMA_DCR_DSIZE(0);

//	NVIC_SetPriority(DMA0_IRQn, 3);
//	NVIC_ClearPendingIRQ(DMA0_IRQn);
//	NVIC_EnableIRQ(DMA0_IRQn);

//	while(1)
//	{
//		vTaskSuspend(NULL);
//	}
}

void dma_transfer(uint32_t* srcAddr,
                  uint32_t* destAddr,
                  uint32_t transferCount,
				  dma_callback inCallback)
{
	LOG_STRING(LOG_MODULE_MAIN, LOG_SEVERITY_STATUS, "DMA transfer.");
	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) srcAddr);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) destAddr);
	// initialize byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(transferCount*sizeof(uint32_t));
	// clear done flag and status flags
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK;

	sCurrentCallback = inCallback;

	// start transfer
	DMA0->DMA[0].DCR |= DMA_DCR_START_MASK;

	// wait until it is done
	while (!(DMA0->DMA[0].DSR_BCR & DMA_DSR_BCR_DONE_MASK))
		;

	sCurrentCallback();

//	while(!g_DMADoneFlag);
//	g_DMADoneFlag = false;

}
