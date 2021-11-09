#include <Arduino.h>

/**
 * Get N-th token from string containing tokens separated by specified separator.
 * 
 * @param dst - bestination char buffer into which token will be copied
 * @param dstSize - size of destination buffer - should be large enough for all possible tokens
 * @param src - source string containing delimited tokens
 * @param separator - token separator
 * @param pos - which token should be returned
 * 
 * @return token length, 0 if not or token is too large to fit destination buffer
 */

int getToken(char* dst, size_t dstSize, const char* src, char separator, int pos);

