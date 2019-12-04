/*
 * @file uart.c
 * @brief Project 5
 *
 * @details Contains interface for UART communications.
 *
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 *  LEVERAGED CODE:
 *  https://github.com/alexander-g-dean/ESF/tree/master/Code/Chapter_8/Serial-Demo
 */

#include "uart.h"
#include "handle_led.h"
#include "circular_buffer.h"
#include <stddef.h>

/**
 * How many characters we want to initially request for the circular buffer
 */
#define UART_CAPACITY 256

/**
 * Transmit circular buffer
 */
static cbuf_handle_t sTxBuffer = NULL;

/**
 * Receive circular buffer
 */
static cbuf_handle_t sRxBuffer = NULL;

/**
 * Enable interrupt macro
 */
#define ENABLE_IRQ NVIC_EnableIRQ(UART0_IRQn);

/**
 * Disable interrupt macro
 */
#define DISABLE_IRQ NVIC_DisableIRQ(UART0_IRQn);

void uart_init(int64_t baud_rate)
{
	 set_led(1, BLUE);

	uint16_t sbr;
	uint8_t temp;

	// Enable clock gating for UART0 and Port A
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

	// Make sure transmitter and receiver are disabled before init
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK;

	// Set UART clock to 48 MHz clock
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;

	// Set pins to UART0 Rx and Tx
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Rx
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Tx

	// Set baud rate and oversampling ratio
	sbr = (uint16_t)((SYS_CLOCK/2)/(baud_rate * UART_OVERSAMPLE_RATE));
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
	UART0->BDL = UART0_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);

	// Disable interrupts for RX active edge and LIN break detect, select one stop bit
	UART0->BDH |= UART0_BDH_RXEDGIE(0) | UART0_BDH_SBNS(0) | UART0_BDH_LBKDIE(0);

	// Don't enable loopback mode, use 8 data bit mode, don't use parity
	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(0) | UART0_C1_PE(0);
	// Don't invert transmit data, don't enable interrupts for errors
	UART0->C3 = UART0_C3_TXINV(0) | UART0_C3_ORIE(0)| UART0_C3_NEIE(0)
			| UART0_C3_FEIE(0) | UART0_C3_PEIE(0);

	// Clear error flags
	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);

	// Try it a different way
	UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK |
									UART0_S1_FE_MASK | UART0_S1_PF_MASK;

	// Send LSB first, do not invert received data
	UART0->S2 = UART0_S2_MSBF(0) | UART0_S2_RXINV(0);

#if USE_UART_INTERRUPTS
	// Enable interrupts. Listing 8.11 on p. 234
	sTxBuffer = circular_buf_init(UART_CAPACITY);
	sRxBuffer = circular_buf_init(UART_CAPACITY);

	NVIC_SetPriority(UART0_IRQn, 2); // 0, 1, 2, or 3
	NVIC_ClearPendingIRQ(UART0_IRQn);
	ENABLE_IRQ

	// Enable receive interrupts but not transmit interrupts yet
	// also turn on error interrupts
	UART0->C2 |= UART_C2_RIE(1) |
			     UART_C2_TIE(1);

	UART0->C3 |= UART_C3_ORIE(1) |
				 UART_C3_NEIE(1) |
				 UART_C3_PEIE(1) |
				 UART_C3_FEIE(1);
#endif

	// Enable UART receiver and transmitter
	UART0->C2 |= UART0_C2_RE(1) | UART0_C2_TE(1);

	// Clear the UART RDRF flag
	temp = UART0->D;
	UART0->S1 &= ~UART0_S1_RDRF_MASK;

}

bool uart_putchar_space_available ()
{
	set_led(1, GREEN);
    return (UART0->S1 & UART0_S1_TDRE_MASK);
}

bool uart_getchar_present ()
{
	//set_led(1, BLUE);
    return (UART0->S1 & UART0_S1_RDRF_MASK);
}

void uart_putchar (char ch)
{
	 set_led(1, GREEN);
    /* Wait until space is available in the FIFO */
    while(!(UART0->S1 & UART0_S1_TDRE_MASK));

    /* Send the character */
    UART0->D = (uint8_t)ch;
}

bool uart_getchar(uint8_t* outChar)
{
	 set_led(1, BLUE);

	/* Return the 8-bit data from the receiver */
	if((UART0->S1 & UART0_S1_RDRF_MASK))
	{
		*outChar = UART0->D;
		return true;
	}
	return false;
}

// taken from DEAN
void uart_put_string(const char* str) {
	// enqueue string
	while (*str != '\0') { // Send characters up to null terminator
		uart_putchar(*str++);
	}
}

bool uart_echo(uint8_t* outChar)
{
#if USE_UART_INTERRUPTS
	if(circular_buf_pop(sRxBuffer, outChar) == buff_err_success)
	{
		circular_buf_push_resize(&sTxBuffer, *outChar);
		UART0->C2 |= UART0_C2_TIE_MASK;
		return true;
	}
#else
	uint8_t ch;

	if(uart_getchar(&ch))
	{
		*outChar = ch;
		uart_putchar(ch);
		return true;
	}

#endif
	return false;
}

// UART0 IRQ Handler. Listing 8.12 on p. 235
void UART0_IRQHandler(void) {
	DISABLE_IRQ

	uint8_t ch;

	// error handling
	if (UART0->S1 & (UART_S1_OR_MASK |UART_S1_NF_MASK |
		             UART_S1_FE_MASK | UART_S1_PF_MASK))
	{
			// clear the error flags
			UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK |
						 UART0_S1_FE_MASK | UART0_S1_PF_MASK;
			// read the data register to clear RDRF
			ch = UART0->D;

			 set_led(1, RED);
	}

	// received a character
	if (UART0->S1 & UART0_S1_RDRF_MASK)
	{

		ch = UART0->D;
		if (!circular_buf_full(sRxBuffer))
		{
			circular_buf_push_resize(&sRxBuffer, ch);
		}
		else
		{
			// error - queue full.
			// discard character
		}

		 set_led(1, BLUE);
	}

	// transmitter interrupt enabled and tx buffer empty
	if ( (UART0->C2 & UART0_C2_TIE_MASK) &&
			(UART0->S1 & UART0_S1_TDRE_MASK) )
	{
		// can send another character
		if (!circular_buf_empty(sTxBuffer))
		{
			uint32_t outCh = -1;
			if(circular_buf_pop(sTxBuffer, &outCh) == buff_err_success)
			{
			  UART0->D = outCh;
			}
		}
		else
		{
			// queue is empty so disable transmitter interrupt
			UART0->C2 &= ~UART0_C2_TIE_MASK;
		}
		 set_led(1, GREEN);
	}

	ENABLE_IRQ
}


