#ifndef LOGGER_HEADER
#define LOGGER_HEADER

#include <stdio.h>

void Log(FILE * const stream, const char * prefix, const char * const format, const char * suffix, va_list arguments);

void LogDebug(const char * const format, ...);

void LogError(const char * const format, ...);

void LogErrorRaw(const char * const format, ...);

void LogInfo(const char * const format, ...);

#endif
