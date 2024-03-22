#ifndef LOGGER_HEADER
#define LOGGER_HEADER

#include "Environment.h"
#include "String.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Platform dependent macros.
 *
 * @see https://sourceforge.net/p/predef/wiki/OperatingSystems/
 */
#if (defined (__APPLE__) && defined (__MACH__)) \
	|| defined (Macintosh) \
	|| defined (macintosh)
#define CRITICAL_COLOR "\033[0;31m"
#define DEBUGGING_COLOR "\033[0;35m"
#define DEFAULT_COLOR "\033[0m"
#define ERROR_COLOR "\033[1;31m"
#define INFORMATION_COLOR "\033[0;32m"
#define WARNING_COLOR "\033[0;33m"
#elif defined (__GNU__) \
	|| defined (__gnu_linux__) \
	|| defined (__linux__) \
	|| defined (__linux) \
	|| defined (linux)
#define CRITICAL_COLOR "\033[0;31m"
#define DEBUGGING_COLOR "\033[0;35m"
#define DEFAULT_COLOR "\033[0m"
#define ERROR_COLOR "\033[1;31m"
#define INFORMATION_COLOR "\033[0;32m"
#define WARNING_COLOR "\033[0;33m"
#elif defined (__TOS_WIN__) \
	|| defined (__WIN32__) \
	|| defined (__WINDOWS__) \
	|| defined (_WIN16) \
	|| defined (_WIN32) \
	|| defined (_WIN64)
#define CRITICAL_COLOR "\x1B[31m"
#define DEBUGGING_COLOR "\x1B[35m"
#define DEFAULT_COLOR "\x1B[0m"
#define ERROR_COLOR "\x1B[91m"
#define INFORMATION_COLOR "\x1B[92m"
#define WARNING_COLOR "\x1B[33m"
#else
#define CRITICAL_COLOR ""
#define DEBUGGING_COLOR ""
#define DEFAULT_COLOR ""
#define ERROR_COLOR ""
#define INFORMATION_COLOR ""
#define WARNING_COLOR ""
#endif

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
