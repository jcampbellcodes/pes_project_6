/*
 * @file circular_buffer.c
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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "circular_buffer.h"
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

/**
 * ABS
 * @details Leveraged code in this file includes the ABS macro, taken from
 *          Slide 30 of "More C Topics" lecture from ECEN 5813
 *          Link: https://canvas.colorado.edu/courses/53078/files/folder/Class%20FIles?preview=7085601
 *  Takes a number and returns the absolute value of that number.
 */
#define ABS(x) ((x)>0?(x):-(x))

/**
 * @brief List node for keeping track of owned buffers
 */
struct mem_list_node
{
	circular_buf_t* data;
	size_t size;
	struct mem_list_node* next;
};

// sentinel struct, always has null data
static struct mem_list_node memListHead = { NULL, 0, NULL };

/**
 * @brief Whether the given handle is owned currently or null or garbage.
 * @param inHandle Handle to check.
 * @return
 */
bool bufferIsOwned(cbuf_handle_t inHandle)
{
	circular_buf_t* buffer = (circular_buf_t*)inHandle;
	if(buffer)
	{
		// try and see if the requested location resides in owned memory
		struct mem_list_node* iter = &memListHead;
		while (iter)
		{
			if(iter->data == buffer)
			{
				return true;
			}
			iter = iter->next;
		}
	}
	return false;
}

cbuf_handle_t circular_buf_init(size_t inSize)
{
	assert(inSize);

	struct mem_list_node* iter = &memListHead;

	while(iter->next)
	{
		iter = iter->next;
	}
	iter->next = (struct mem_list_node*)malloc(sizeof(struct mem_list_node));

	iter->next->data = (circular_buf_t*)malloc(sizeof(circular_buf_t));

	assert(iter->next->data);

	iter->next->data->buffer = (uint8_t*)malloc(sizeof(uint8_t)*inSize);
	assert(iter->next->data->buffer);
	iter->next->data->max = inSize;
	iter->next->data->write = 0;
	iter->next->data->read = 0;
	iter->next->data->full = false;

	iter->next->next = NULL;

	assert(circular_buf_empty(iter->next->data));
	return iter->next->data;
}

void circular_buf_free(cbuf_handle_t inBufHandle)
{
	circular_buf_t* buffer = (circular_buf_t*)inBufHandle;
	if(buffer)
	{
		struct mem_list_node* iter = &memListHead;

		while (iter->next)
		{
			if(iter->next->data == buffer)
			{
				free(buffer->buffer);
				free(buffer);
				struct mem_list_node* node_to_delete = iter->next;
				iter->next = iter->next->next;
				free(node_to_delete);
				return;
			}
			iter = iter->next;
		}
	}
}

buff_err circular_buf_resize(cbuf_handle_t* inOutBufHandle, size_t inSize)
{
	if(inOutBufHandle &&
	   bufferIsOwned(*inOutBufHandle) &&
	   inSize > circular_buf_size(*inOutBufHandle))
	{
		// create new buffer
		cbuf_handle_t newBuf = circular_buf_init(inSize);

		// copy contents from old buffer
		uint8_t ch;
		while(circular_buf_pop(*inOutBufHandle, &ch) == buff_err_success)
		{
			circular_buf_push(newBuf, ch);
		}

		// free old buffer
		circular_buf_free(*inOutBufHandle);

		// set output buffer
		*inOutBufHandle = newBuf;

		return buff_err_success;
	}
	return buff_err_invalid;
}

buff_err circular_buf_push(cbuf_handle_t inBufHandle, uint8_t inData)
{
	buff_err err = buff_err_invalid;

	if(bufferIsOwned(inBufHandle))
	{
		if(circular_buf_full(inBufHandle))
			return buff_err_full;

		inBufHandle->buffer[inBufHandle->write] = inData;
		if(inBufHandle->full)
		{
			inBufHandle->read = (inBufHandle->read + 1) % inBufHandle->max;
		}

		inBufHandle->write = (inBufHandle->write + 1) % inBufHandle->max;
		inBufHandle->full = (inBufHandle->write == inBufHandle->read);
		err = buff_err_success;

	}
	return err;
}

buff_err circular_buf_push_resize(cbuf_handle_t* inOutBufHandle, uint8_t inData)
{
	buff_err err = buff_err_invalid;
	if(inOutBufHandle)
	{
		cbuf_handle_t inBufHandle = *inOutBufHandle;

		if(bufferIsOwned(inBufHandle))
		{
			if(circular_buf_full(inBufHandle))
			{
				if(circular_buf_resize(inOutBufHandle, inBufHandle->max * 2) == buff_err_success)
				    inBufHandle = *inOutBufHandle;
				else
					return err;
			}

			inBufHandle->buffer[inBufHandle->write] = inData;
			if(inBufHandle->full)
			{
				inBufHandle->read = (inBufHandle->read + 1) % inBufHandle->max;
			}

			inBufHandle->write = (inBufHandle->write + 1) % inBufHandle->max;
			inBufHandle->full = (inBufHandle->write == inBufHandle->read);
			err = buff_err_success;

		}
	}

	return err;
}

buff_err circular_buf_pop(cbuf_handle_t inBufHandle, uint8_t * outData)
{
	buff_err err = buff_err_invalid;
	if(bufferIsOwned(inBufHandle))
	{
		if(circular_buf_empty(inBufHandle))
			return buff_err_empty;

		*outData = inBufHandle->buffer[inBufHandle->read];
		inBufHandle->full = false;
		inBufHandle->read = (inBufHandle->read + 1) % inBufHandle->max;
		err = buff_err_success;
	}
	return err;
}

bool circular_buf_empty(cbuf_handle_t inBufHandle)
{
	assert(bufferIsOwned(inBufHandle));
    return (!inBufHandle->full && (inBufHandle->write == inBufHandle->read));
}

bool circular_buf_full(cbuf_handle_t inBufHandle)
{
	assert(bufferIsOwned(inBufHandle));
    return inBufHandle->full;
}

size_t circular_buf_capacity(cbuf_handle_t inBufHandle)
{
	assert(bufferIsOwned(inBufHandle));
	return inBufHandle->max;
}

size_t circular_buf_size(cbuf_handle_t inBufHandle)
{
	assert(bufferIsOwned(inBufHandle));

	size_t size = inBufHandle->max;

	if(!inBufHandle->full)
	{
		if(inBufHandle->write >= inBufHandle->read)
		{
			size = (inBufHandle->write - inBufHandle->read);
		}
		else
		{
			size = (inBufHandle->max + inBufHandle->write - inBufHandle->read);
		}
	}

	return size;
}
