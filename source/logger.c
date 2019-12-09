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

#include "time.h"
#include "uart.h"

/**
 * Used as a size for static char arrays.
 */
#define ARRLEN 500

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
		"DMA",
		"SETUP_TEARDOWN",
		"CIRCULAR_BUFFER",
		"TASKS",
		"TIME",
		"POST",
        "UART",
		"SINE"
};

/**
 * @brief Prints the current time stamp in HH:MM:SS.n format
 */
static void PRINT_TIME_STAMP()
{
	timestamp_str timestamp;
	timestamp_now(&timestamp);

	uart_put_string(timestamp.hours);
	uart_put_string(timestamp.mins);
	uart_put_string(timestamp.secs);
	uart_put_string(timestamp.tens);
}

/**
 * @brief Used to standardize the prefix before log messages.
 */
static void PRINT_LOG_PREFIX(LogModule_t inModule, const char* inFuncName, LogSeverity_t inSeverity)
{
	static char format_buf[ARRLEN] = {0};
	for(int i = 0; i < ARRLEN; i++) format_buf[i] = '\0';

	uart_put_string("\n\r");
	sprintf(format_buf, "%s -> %s::[%s] : ",  sLogSeverityStrings[inSeverity] , sLogModuleStrings[inModule], inFuncName);
	PRINT_TIME_STAMP();
	uart_put_string(format_buf);
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
}

void log_disable()
{
	sLoggingEnabled = false;
}

bool log_enabled()
{
	return sLoggingEnabled;
}

void log_string(LogModule_t inModule, const char* inFuncName, LogSeverity_t inSeverity, const char* inString, ...)
{
	char format_buf[ARRLEN] = {0};
	for(int i = 0; i < ARRLEN; i++) format_buf[i] = '\0';

	if (sLoggingEnabled && inSeverity >= sLogSeverity) {

	    va_list argp;
	    va_start(argp, inString);
	    vsprintf(format_buf, inString, argp);
	    va_end(argp);
	    PRINT_LOG_PREFIX(inModule, inFuncName, inSeverity);

	    uart_put_string(format_buf);
	    uart_put_string("\n\r");
	}
}

