/*
 * @file logger.h
 * @brief Project 5
 *
 * Tools for logging.
 *
 * @author Jack Campbell
 * @tools  PC Compiler: GNU gcc 8.3.0
 *         PC Linker: GNU ld 2.32
 *         PC Debugger: GNU gdb 8.2.91.20190405-git
 *         ARM Compiler: GNU gcc version 8.2.1 20181213
 *         ARM Linker: GNU ld 2.31.51.20181213
 *         ARM Debugger: GNU gdb 8.2.50.20181213-git
 */

#include "logger.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

#include <stdio.h>
#include <stdarg.h>
#include "fsl_debug_console.h"
#include "time.h"
#include "board.h"

/**
 * Used as a size for static char arrays.
 */
#define ARRLEN 1000

/**
 * @brief Strings associated with severities.
 */
static const char* sLogSeverityStrings[NUM_LOG_SEVERITIES] =
{
	"TEST",
	"DEBUG",
	"STATUS"
};

/**
 * @brief Strings associated with modules.
 */
static const char* sLogModuleStrings[NUM_LOG_MODULES] =
{
		"MAIN",
		"LED",
		"UNIT_TEST",
		"SETUP_TEARDOWN",
		"CIRCULAR_BUFFER",
		"TIME",
		"POST",
        "UART"
};

/**
 * @brief Prints the current time stamp in HH:MM:SS.n format
 */
static void PRINT_TIME_STAMP()
{
	static char format_buf[ARRLEN] = {0};
	for(int i = 0; i < ARRLEN; i++) format_buf[i] = '\0';

	uint64_t tenths_seconds = time_now();

	float now = tenths_seconds / 10;

	uint64_t hours = (uint64_t)(now/3600)%60;
	uint64_t minutes = (uint64_t)(now/60)%60;
	uint64_t seconds = (uint64_t)(now)%60;

	sprintf(format_buf, "%02d:",  hours);
	PRINTF(format_buf);
	sprintf(format_buf, "%02d:",  minutes);
	PRINTF(format_buf);
	sprintf(format_buf, "%02d",  seconds);
	PRINTF(format_buf);
	sprintf(format_buf, ".%1d ",  tenths_seconds%10);
	PRINTF(format_buf);
}

/**
 * @brief Used to standardize the prefix before log messages.
 */
static void PRINT_LOG_PREFIX(LogModule_t inModule, const char* inFuncName, LogSeverity_t inSeverity)
{
	PRINTF("\n\r");
	PRINTF("%s -> %s::[%s] : ",  sLogSeverityStrings[inSeverity] , sLogModuleStrings[inModule], inFuncName);
	PRINT_TIME_STAMP();
}

/**
 * @brief Static variable maintains the logging state.
 */
static bool sLoggingEnabled = false;

/**
 * @brief Static severity maintains the severity for the module.
 */
static LogSeverity_t sLogSeverity = LOG_SEVERITY_STATUS;

void log_enable(LogSeverity_t inSeverity)
{
	sLoggingEnabled = true;
	sLogSeverity = inSeverity;
	BOARD_InitDebugConsole();
}

void log_disable()
{
	sLoggingEnabled = false;
}

bool log_enabled()
{
	return sLoggingEnabled;
}

void log_data(LogModule_t inModule, const char* inFuncName, LogSeverity_t inSeverity, const uint8_t* inBytes, size_t inSize)
{
	if (sLoggingEnabled && inSeverity >= sLogSeverity)
	{
		PRINT_LOG_PREFIX(inModule, inFuncName, inSeverity);
		PRINTF("\n\rBytes at address %p:\n\r==========================\n\r", inBytes);
		for(int i = 0; i < inSize; i++)
		{
			PRINTF("%2x ", inBytes[i]);
			if((i+1)%4 == 0)
			{
				PRINTF("\r\n");
			}
		}
		printf("\n\r==========================\n\r");
	}
}


void log_string(LogModule_t inModule, const char* inFuncName, LogSeverity_t inSeverity, const char* inString, ...)
{
	static char format_buf[ARRLEN] = {0};
	for(int i = 0; i < ARRLEN; i++) format_buf[i] = '\0';

	if (sLoggingEnabled && inSeverity >= sLogSeverity) {

	    va_list argp;
	    va_start(argp, inString);
	    vsprintf(format_buf, inString, argp);
	    va_end(argp);
	    PRINT_LOG_PREFIX(inModule, inFuncName, inSeverity);
		PRINTF("%s\n\r", format_buf);
	}
}


void log_integer(LogModule_t inModule, const char* inFuncName, LogSeverity_t inSeverity, uint64_t inNum)
{
	if (sLoggingEnabled && inSeverity >= sLogSeverity)
	{
		PRINT_LOG_PREFIX(inModule, inFuncName, inSeverity);
		PRINTF("%lld\n\r", inNum);
	}
}
