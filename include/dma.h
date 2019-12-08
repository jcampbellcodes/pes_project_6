#ifndef __dmah__
#define __dmah__
#include <stdint.h>

/*! @brief Callback function prototype for the DMA driver. */
// taken from the SDK
typedef void (*dma_callback)();

// void* so that the init can be a task
void dma_init(void* cookie);

void dma_transfer(uint32_t* srcAddr,
        uint32_t* destAddr,
        uint32_t transferBytes,
		dma_callback inCallback);

#endif
