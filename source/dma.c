#include "dma.h"
//#include "fsl_dma.h"
//#include "fsl_dmamux.h"
#include "MKL25Z4.h"
#include "logger.h"
dma_handle_t g_DMA_Handle;

// taken from https://github.com/alexander-g-dean/ESF/blob/master/Code/Chapter_9/DMA_Examples/Source/DMA.c

void dma_init()
{
//    /* Configure DMAMUX */
//    DMAMUX_Init(DMAMUX0);
//    DMAMUX_SetSource(DMAMUX0, 0, 0);
//    DMAMUX_EnableChannel(DMAMUX0, 0);
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	DMA0->DMA[0].DCR = DMA_DCR_SINC_MASK | DMA_DCR_SSIZE(0) |
		DMA_DCR_DINC_MASK |	DMA_DCR_DSIZE(0);
}

void dma_transfer(uint32_t* srcAddr,
                  uint32_t* destAddr,
                  uint32_t transferCount)
{
//    dma_transfer_config_t transferConfig;
//    /* Configure DMA one shot transfer */
//    DMA_Init(DMA0);
//    DMA_CreateHandle(&g_DMA_Handle, DMA0, 0U);
//    DMA_SetCallback(&g_DMA_Handle, dmaCallback, NULL);
//    DMA_PrepareTransfer(&transferConfig, srcAddr, sizeof(uint32_t), destAddr, sizeof(uint32_t), transferBytes,
//                        kDMA_MemoryToMemory);
//    if(DMA_SubmitTransfer(&g_DMA_Handle, &transferConfig, kDMA_EnableInterrupt) != kStatus_Success)
//    {
//    	LOG_STRING(LOG_MODULE_SETUP_TEARDOWN, LOG_SEVERITY_DEBUG, "DMA Transfer Failed!!");
//    }
//    DMA_StartTransfer(&g_DMA_Handle);


	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) srcAddr);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) destAddr);
	// initialize byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(transferCount*sizeof(uint32_t));
	// clear done flag and status flags
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK;

	// start transfer
	//Control_RGB_LEDs(0,0,1); TODO: Turn on LEDs here?
	DMA0->DMA[0].DCR |= DMA_DCR_START_MASK;
	// mark start of transfer

	// wait until it is done
	while (!(DMA0->DMA[0].DSR_BCR & DMA_DSR_BCR_DONE_MASK))
		;
	// mark end of transfer
	// Control_RGB_LEDs(0,0,0); TODO: Turn off leds here?
}
