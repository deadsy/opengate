 //-----------------------------------------------------------------------------
/*

Utilities

*/
//-----------------------------------------------------------------------------

#include "util.h"

//-----------------------------------------------------------------------------

// 4-bit value to character.
char hex4_char(int val) {
	val &= 15;
	if (val >= 10 && val <= 15) {
		return 'A' + val - 10;
	}
	return '0' + val;
}

// 4-bit value to string.
char *hex4(char *s, int val) {
	s[0] = hex4_char(val);
	s[1] = 0;
	return s;
}

// 8-bit value to string.
char *hex8(char *s, uint8_t val) {
	hex4(s, val >> 4);
	hex4(&s[1], val);
	return s;
}

// 16-bit value to string.
char *hex16(char *s, uint16_t val) {
	hex8(s, val >> 8);
	hex8(&s[2], val);
	return s;
}

// 32-bit value to string.
char *hex32(char *s, uint32_t val) {
	hex16(s, val >> 16);
	hex16(&s[4], val);
	return s;
}

//-----------------------------------------------------------------------------

// integer to string
char *itoa(char *s, int val) {
	unsigned int uval = (val < 0) ? -val : val;
	// to decimal
	int i = 0;
	do {
		s[i++] = '0' + (uval % 10);
		uval /= 10;
	} while (uval);
	// add negative
	if (val < 0) {
		s[i++] = '-';
	}
	// terminate
	s[i--] = 0;
	// reverse
	int j = 0;
	while (j < i) {
		char c = s[i];
		s[i] = s[j];
		s[j] = c;
		j++;
		i--;
	}
	return s;
}

//-----------------------------------------------------------------------------
