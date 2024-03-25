#ifndef FLEX_EXPORT_HEADER
#define FLEX_EXPORT_HEADER

/**
 * Hook that allows to export a static function or variable from the inside of
 * Flex infrastructure, in this case, the current context (a.k.a. start
 * condition).
 */
unsigned int flexCurrentContext(void) {
	return YY_START;
}

#endif
