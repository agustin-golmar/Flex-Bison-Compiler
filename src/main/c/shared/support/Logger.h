#ifndef LOGGER_HEADER
#define LOGGER_HEADER

#include "Environment.h"
#include "String.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * The available logging levels. Highest levels represents worst failures;
 * lower levels provides greater traceability.
 */
typedef enum {
	ALL = 0,
	DEBUGGING = 10,
	INFORMATION = 20,
	WARNING = 30,
	ERROR = 40,
	CRITICAL = 50
} LoggingLevel;

/**
 * The definition of a logger. It contains a name to see its traces, and a
 * logging level that defines when the logs must be visible.
*/
typedef struct {
	LoggingLevel loggingLevel;
	char * name;
} Logger;

/**
 * Creates a new logger with the specified name. That name will be visible in
 * every line you log with this object.
 */
Logger * createLogger(char * name);

/**
 * Destroy a logger and its resources.
 */
void destroyLogger(Logger * logger);

/** Logs at CRITICAL level. */
void logCritical(const Logger * logger, const char * const format, ...);

/** Logs at DEBUGGING level. */
void logDebugging(const Logger * logger, const char * const format, ...);

/** Logs at ERROR level. */
void logError(const Logger * logger, const char * const format, ...);

/** Logs at INFORMATION level. */
void logInformation(const Logger * logger, const char * const format, ...);

/** Logs at WARNING level. */
void logWarning(const Logger * logger, const char * const format, ...);

#endif
