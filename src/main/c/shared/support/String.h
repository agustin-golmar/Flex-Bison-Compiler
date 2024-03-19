#ifndef STRING_HEADER
#define STRING_HEADER

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/**
 * Concatenates a list of strings. The returned string must be freed because
 * it uses heap-memory.
 *
 * @param count
 *	The size of the list.
 */
char * concatenate(const unsigned int count, ...);

/**
 * Creates a new string (using heap-memory), that is equal to the original,
 * but using escaped sequences for every control character (i.e., in the range
 * 0x00-0x1F and 0x7F).
 */
char * escape(const char * string);

#endif
