/*
 * @file dma.h
 * @brief Project 6
 *
 * @details This file contains code for using the DMA controller.
 *
 * @author Jack Campbell
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 *
 */

#ifndef __dmah__
#define __dmah__
#include <stdint.h>

/**
 * A callback type to pass to the DMA transfer.
 */
typedef void (*dma_callback)();

// void* so that the init can be a task

/**
 * DMA init.
 * \note This function takes a void* so that it can be run as a task.
 * I read that enabling interrupts must be done after the scheduler starts,
 * but I am not sure whether this is true.
 */
void dma_init(void* cookie);

/**
 * Request a DMA transfer.
 * \param srcAddr The address to transfer from.
 * \param destAddr The address to transfer to.
 * \param tansferBytes Number of bytes to transfer.
 * \param inCallback A callback to fire when the transfer completes.
 */
void dma_transfer(uint32_t* srcAddr,
        uint32_t* destAddr,
        uint32_t transferBytes,
		dma_callback inCallback);

#endif
