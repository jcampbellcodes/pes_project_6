/*
 * @file circular_buffer.h
 * @brief Project 5
 *
 * @details This file contains code for a circular buffer.
 *
 * @author Jack Campbell
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 *
 *  LEVERAGED API AND IMPLEMENTATION FROM:
 *  https://embeddedartistry.com/blog/2017/05/17/creating-a-circular-buffer-in-c-and-c/
 */


#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Buffer error codes.
 */
typedef enum buff_err {
	buff_err_success,
	buff_err_full,
	buff_err_empty,
	buff_err_invalid
} buff_err;

/**
 * @brief Opaque struct for circular buffer
 */
typedef struct circular_buf_t {
	uint8_t * buffer;
	size_t write;
	size_t read;
	size_t max;
	bool full;
} circular_buf_t;

/**
 * @brief Circular buffer handle type to use with free functions
 */
typedef circular_buf_t* cbuf_handle_t;

/**
 * @brief Create a new buffer
 * @param inSize Capacity of the buffer
 * @return A circular buffer handle
 */
cbuf_handle_t circular_buf_init(size_t inSize);

/**
 * @brief Free a circular buffer and all associated heap memory
 * @param inBufHandle Handle to the buffer to free
 */
void circular_buf_free(cbuf_handle_t inBufHandle);

/**
 * @brief Resize a circular buffer
 * @param inOutBufHandle The buffer to resize
 * @param inSize The size to resize to
 * @return Whether the operation succeeded
 */
buff_err circular_buf_resize(cbuf_handle_t* inOutBufHandle, size_t inSize);

/**
 * @brief Push a new element into the circular buffer
 * @param inBufHandle Buffer to push to
 * @param inData Data to push into the buffer
 * @return Whether the operation succeeded. Errors if not.
 */
buff_err circular_buf_push(cbuf_handle_t inBufHandle, uint8_t inData);

/**
 * @brief Push a new element to the circular buffer, resizing if full
 * @param inOutBufHandle A pointer to the handle to push to
 * @param inData The data to push
 * @return Whether the operation succeeded
 */
buff_err circular_buf_push_resize(cbuf_handle_t* inOutBufHandle, uint8_t inData);

/**
 * @brief Pop an element from the buffer
 * @param inBufHandle The buf to access
 * @param outData The data accessed
 * @return Whether the operation was successful. Error if empty.
 */
buff_err circular_buf_pop(cbuf_handle_t inBufHandle, uint8_t * outData);

/**
 * @brief Return whether the buffer is empty
 * @param inBufHandle The buffer to check
 * @return
 */
bool circular_buf_empty(cbuf_handle_t inBufHandle);

/**
 * @brief Whether the buffer is full
 * @param inBufHandle The buffer to check
 * @return
 */
bool circular_buf_full(cbuf_handle_t inBufHandle);

/**
 * @brief Capacity of the buffer
 * @param inBufHandle The buffer to check
 * @return
 */
size_t circular_buf_capacity(cbuf_handle_t inBufHandle);

/**
 * @brief The number of elements in the buffer
 * @param inBufHandle The buffer to check
 * @return
 */
size_t circular_buf_size(cbuf_handle_t inBufHandle);

#endif

