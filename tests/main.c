#include "uCUnit.h"
#include <stdint.h>
#include "logger.h"
#include "setup_teardown.h"
#include "circular_buffer.h"
#include "handle_led.h"

#define HANDLE_ERROR(num_errors) if(num_errors > 0) set_led(1, RED); else set_led(1, GREEN);

#define TEST_BUF_SIZE 16

int main()
{
	initialize();
	set_led(1, BLUE);

	{
	    UCUNIT_TestcaseBegin("Create a buffer");
	    cbuf_handle_t buf = circular_buf_init(TEST_BUF_SIZE);
        UCUNIT_CheckIsNotNull(buf->buffer);
        UCUNIT_CheckIsEqual(buf->write, 0);
        UCUNIT_CheckIsEqual(buf->read, 0);
        UCUNIT_CheckIsEqual(buf->max, TEST_BUF_SIZE);
        UCUNIT_CheckIsEqual(buf->full, false);
	    UCUNIT_TestcaseEnd();
	    circular_buf_free(buf);
	}

	{
		UCUNIT_TestcaseBegin("Destroy buffer");
		cbuf_handle_t buf = circular_buf_init(TEST_BUF_SIZE);
		circular_buf_free(buf);
	    uint8_t outbyte = 0x0;
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf, &outbyte), buff_err_invalid);
		UCUNIT_TestcaseEnd();
	}

	{
		UCUNIT_TestcaseBegin("Destroy the same buffer twice");
		cbuf_handle_t buf = circular_buf_init(TEST_BUF_SIZE);
		circular_buf_free(buf);
		circular_buf_free(buf);
	    uint8_t outbyte = 0x0;
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf, &outbyte), buff_err_invalid);
		UCUNIT_TestcaseEnd();
	}

	{
		UCUNIT_TestcaseBegin("Test data access");
	    cbuf_handle_t buf = circular_buf_init(TEST_BUF_SIZE);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xAA), buff_err_success);
	    uint8_t outbyte = 0x0;
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf, &outbyte), buff_err_success);
	    UCUNIT_CheckIsEqual(outbyte, 0xAA);
	    circular_buf_free(buf);
		UCUNIT_TestcaseEnd();
	}

	{
		UCUNIT_TestcaseBegin("Test data multiple buffers");
	    cbuf_handle_t buf1 = circular_buf_init(TEST_BUF_SIZE);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf1, 0xAA), buff_err_success);
	    uint8_t outbyte = 0x0;
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf1, &outbyte), buff_err_success);
	    UCUNIT_CheckIsEqual(outbyte, 0xAA);

	    cbuf_handle_t buf2 = circular_buf_init(TEST_BUF_SIZE);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf2, 0xAA), buff_err_success);
	    outbyte = 0x0;
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf2, &outbyte), buff_err_success);
	    UCUNIT_CheckIsEqual(outbyte, 0xAA);


	    circular_buf_free(buf1);
	    circular_buf_free(buf2);
		UCUNIT_TestcaseEnd();
	}


	// test that your buffer can wrap around the edge boundary and add to the front
	{
		UCUNIT_TestcaseBegin("Verify wrap write");

	    cbuf_handle_t buf = circular_buf_init(4);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xAA), buff_err_success);
	    UCUNIT_CheckIsEqual(buf->write, 1);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xBB), buff_err_success);
	    UCUNIT_CheckIsEqual(buf->write, 2);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xCC), buff_err_success);
	    UCUNIT_CheckIsEqual(buf->write, 3);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xDD), buff_err_success);
	    UCUNIT_CheckIsEqual(buf->write, 0);

	    circular_buf_free(buf);

		UCUNIT_TestcaseEnd();
	}

	// test that your buffer tail point can wrap around the edge boundary when a remove occurs at the boundary
	{
		UCUNIT_TestcaseBegin("Verify wrap read");

	    cbuf_handle_t buf = circular_buf_init(4);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xAA), buff_err_success);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xBB), buff_err_success);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xCC), buff_err_success);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xDD), buff_err_success);

	    uint8_t outbyte = 0x0;
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf, &outbyte), buff_err_success);
	    UCUNIT_CheckIsEqual(outbyte, 0xAA);
	    UCUNIT_CheckIsEqual(buf->read, 1);
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf, &outbyte), buff_err_success);
	    UCUNIT_CheckIsEqual(outbyte, 0xBB);
	    UCUNIT_CheckIsEqual(buf->read, 2);
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf, &outbyte), buff_err_success);
	    UCUNIT_CheckIsEqual(outbyte, 0xCC);
	    UCUNIT_CheckIsEqual(buf->read, 3);
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf, &outbyte), buff_err_success);
	    UCUNIT_CheckIsEqual(outbyte, 0xDD);
	    UCUNIT_CheckIsEqual(buf->read, 0);

	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xEE), buff_err_success);
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf, &outbyte), buff_err_success);
	    UCUNIT_CheckIsEqual(outbyte, 0xEE);
	    UCUNIT_CheckIsEqual(buf->read, 1);

	    circular_buf_free(buf);

		UCUNIT_TestcaseEnd();
	}

	// test that your buffer fails when too many items are added
	{
		UCUNIT_TestcaseBegin("Overfill");
	    cbuf_handle_t buf = circular_buf_init(TEST_BUF_SIZE);
	    for(int i = 0; i < TEST_BUF_SIZE; i++)
	    {
	    	UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xAA), buff_err_success);
	    }
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xAA), buff_err_full);
	    circular_buf_free(buf);
		UCUNIT_TestcaseEnd();
	}

	// test that destructive overload resizes buffer when it is full
	{
		UCUNIT_TestcaseBegin("Overfill resize");
	    cbuf_handle_t buf = circular_buf_init(TEST_BUF_SIZE);
	    for(int i = 0; i < TEST_BUF_SIZE; i++)
	    {
	    	UCUNIT_CheckIsEqual(circular_buf_push_resize(&buf, 0xAA), buff_err_success);
	    }
	    UCUNIT_CheckIsEqual(circular_buf_push_resize(&buf, 0xAA), buff_err_success);
	    UCUNIT_CheckIsEqual(buf->max, TEST_BUF_SIZE * 2);
	    circular_buf_free(buf);
		UCUNIT_TestcaseEnd();
	}

	// test that your buffer fails to remove an item when empty
	{
		UCUNIT_TestcaseBegin("Over empty");
	    cbuf_handle_t buf = circular_buf_init(TEST_BUF_SIZE);
	    UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xAA), buff_err_success);
	    uint8_t outbyte = 0x0;
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf, &outbyte), buff_err_success);
	    UCUNIT_CheckIsEqual(outbyte, 0xAA);
	    UCUNIT_CheckIsEqual(circular_buf_pop(buf, &outbyte), buff_err_empty);
	    circular_buf_free(buf);
		UCUNIT_TestcaseEnd();
	}


	{
		UCUNIT_TestcaseBegin("Resize buffer");
	    cbuf_handle_t buf = circular_buf_init(TEST_BUF_SIZE);
	    for(int i = 0; i < TEST_BUF_SIZE; i++)
	    {
	    	UCUNIT_CheckIsEqual(circular_buf_push(buf, 0xAA), buff_err_success);
	    }

	    circular_buf_resize(&buf, TEST_BUF_SIZE*2);
	    UCUNIT_CheckIsEqual(buf->max, TEST_BUF_SIZE * 2);

	    uint8_t ch;
	    for(int i = 0; i < TEST_BUF_SIZE; i++)
	    {
	    	UCUNIT_CheckIsEqual(circular_buf_pop(buf, &ch), buff_err_success);
	    	UCUNIT_CheckIsEqual(ch, 0xAA);
	    }

	    circular_buf_free(buf);
		UCUNIT_TestcaseEnd();
	}


	HANDLE_ERROR(ucunit_testcases_failed)

	terminate();

	return 0;
}
