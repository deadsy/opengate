//-----------------------------------------------------------------------------
/*

Utilities

*/
//-----------------------------------------------------------------------------

#ifndef UTIL_H
#define UTIL_H

//-----------------------------------------------------------------------------

#include <inttypes.h>

//-----------------------------------------------------------------------------

static inline void reg_rmw(volatile uint32_t * reg, uint32_t mask, uint32_t val) {
	uint32_t x = *reg;
	x &= ~mask;
	x |= (val & mask);
	*reg = x;
}

static inline void reg_set(volatile uint32_t * reg, uint32_t bits) {
	reg_rmw(reg, bits, 0xffffffff);
}

static inline void reg_clr(volatile uint32_t * reg, uint32_t bits) {
	reg_rmw(reg, bits, 0);
}

//-----------------------------------------------------------------------------

char hex4_char(int val);
char *hex4(char *s, int val);
char *hex8(char *s, uint8_t val);
char *hex16(char *s, uint16_t val);
char *hex32(char *s, uint32_t val);
char *itoa(char *s, int val);

//-----------------------------------------------------------------------------

#endif				// UTIL_H

//-----------------------------------------------------------------------------
