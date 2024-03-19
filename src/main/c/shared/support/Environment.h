#ifndef ENVIRONMENT_HEADER
#define ENVIRONMENT_HEADER

#include <stdlib.h>

/**
 * Gets the value of an environment variable by name, or returns a default
 * value if the variable is undefined.
 * 
 * @see https://cplusplus.com/reference/cstdlib/getenv/
 */
const char * getStringOrDefault(const char * name, const char * defaultValue);

#endif
