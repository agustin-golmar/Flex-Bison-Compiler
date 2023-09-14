#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

/**
 * Implementación de "logger.h".
 */

void Log(FILE * const stream, const char * prefix, const char * const format, const char * suffix, va_list arguments) {
	fprintf(stream, "%s", prefix);
	vfprintf(stream, format, arguments);
	fprintf(stream, "%s", suffix);
}

void LogDebug(const char * const format, ...) {
	#ifdef DEBUG
	va_list arguments;
	va_start(arguments, format);
	Log(stdout, "[DEBUG] ", format, "\n", arguments);
	va_end(arguments);
	#endif
}

void LogError(const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	Log(stderr, "[ERROR] ", format, "\n", arguments);
	va_end(arguments);
}

void LogErrorRaw(const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	Log(stderr, "", format, "", arguments);
	va_end(arguments);
}

void LogInfo(const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	Log(stdout, "[INFO ] ", format, "\n", arguments);
	va_end(arguments);
}

void LogRaw(const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	Log(stdout, "", format, "", arguments);
	va_end(arguments);
}

void LogText(const char * text, const int length) {
	for (int i = 0; i < length; ++i) {
		switch (text[i]) {
			case '\0':
				LogRaw("\\0");
				break;
			case '\n':
				LogRaw("\\n");
				break;
			case '\r':
				LogRaw("\\r");
				break;
			case '\t':
				LogRaw("\\t");
				break;
			default:
				LogRaw("%c", text[i]);
		}
	}
}
