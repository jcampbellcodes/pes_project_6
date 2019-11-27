/*
 * @file handle_led.c
 * @brief Project 5
 *
 * Functions for handling the state of an LED.
 *
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 *
 *  LEVERAGED CODE from the in-class competition activity
 *  for LEDs.
 */

#include <stdint.h>
#include "handle_led.h"
#include "MKL25Z4.h"
#include "logger.h"

/**
 * @brief The RED LED pin
 */
#define RED_LED_POS (18U)   // on port B

/**
 * @brief The GREEN LED pin
 */
#define GREEN_LED_POS (19U)// on port B

/**
 * @brief The BLUE LED pin
 */
#define BLUE_LED_POS (1U)    // on port D

/**
 * @brief Set a bit at a specific position
 */
#define MASK(x) (1UL << (x))

static uint8_t gLedReady = 0;

void leds_init()
{
	// led init

	// make 3 pins GPIO
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);
	PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);

	// Set ports to outputs using the data direction register
	PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PDDR |= MASK(BLUE_LED_POS);

	PTB->PSOR = MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PSOR = MASK(BLUE_LED_POS);

	gLedReady = 1;
}

/**
 * set_led
 *
 * @brief Sets the LED state.
 * @details This function controls a physical LED and prints
 *          debug info over UART on debug builds.
 * @param inValue The on/off state of the LED to set.
 * @param inColor The color of the LED to set.
 */
void set_led(uint8_t inValue, enum COLOR inColor)
{
	if(gLedReady)
	{
		switch(inColor)
		{
			case RED:
			{
				PTB->PSOR = MASK(GREEN_LED_POS);
				PTD->PSOR = MASK(BLUE_LED_POS);

				if(inValue)
				{
					PTB->PCOR = MASK(RED_LED_POS);
				}
				else
				{
					PTB->PSOR = MASK(RED_LED_POS);
				}

				break;
			}
			case GREEN:
			{
				PTD->PSOR = MASK(BLUE_LED_POS);
				PTB->PSOR = MASK(RED_LED_POS);

				if(inValue)
				{
					PTB->PCOR = MASK(GREEN_LED_POS);
				}
				else
				{
					PTB->PSOR = MASK(GREEN_LED_POS);
				}
				break;
			}
			case BLUE:
			{
				PTB->PSOR = MASK(GREEN_LED_POS);
				PTB->PSOR = MASK(RED_LED_POS);

				if(inValue)
				{
					PTD->PCOR = MASK(BLUE_LED_POS);
				}
				else
				{
					PTD->PSOR = MASK(BLUE_LED_POS);
				}
				break;
			}
			default:
				 break;
		}
	}
}
