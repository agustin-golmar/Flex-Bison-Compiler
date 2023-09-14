#ifndef LOGGER_HEADER
#define LOGGER_HEADER

#include <stdio.h>

// Descomentar para loguear en modo DEBUG (con más detalle).
#define DEBUG true

void Log(FILE * const stream, const char * prefix, const char * const format, const char * suffix, va_list arguments);

void LogDebug(const char * const format, ...);

void LogError(const char * const format, ...);

void LogErrorRaw(const char * const format, ...);

void LogInfo(const char * const format, ...);

void LogRaw(const char * const format, ...);

void LogText(const char * text, const int length);

#endif
