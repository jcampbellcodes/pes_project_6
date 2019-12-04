#ifndef __dmah__
#define __dmah__
#include <stdint.h>
#include "fsl_dma.h"
#include "fsl_dmamux.h"

void dma_init();

void dma_transfer(uint32_t* srcAddr,
        uint32_t* destAddr,
        uint32_t transferBytes);

#endif
