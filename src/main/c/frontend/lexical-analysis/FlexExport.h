#ifndef FLEX_EXPORT_HEADER
#define FLEX_EXPORT_HEADER

/**
 * Hook that allows to export a static function from the inside of Flex
 * infrastructure.
 */
unsigned int flexCurrentContext(void) {
	return YY_START;
}

#endif
