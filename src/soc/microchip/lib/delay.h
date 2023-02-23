//-----------------------------------------------------------------------------
/*

Polled Delays

*/
//-----------------------------------------------------------------------------

#ifndef DELAY_H
#define DELAY_H

#ifndef SOC_H
#warning "please include this file using the toplevel soc.h"
#endif

//-----------------------------------------------------------------------------

extern volatile uint32_t globalTick;

static inline void incTick(void) {
	globalTick++;
}

static inline uint32_t getTick(void) {
	return globalTick;
}

//-----------------------------------------------------------------------------

void msDelay(unsigned long msecs);
void usDelay(unsigned long usecs);

//-----------------------------------------------------------------------------

#endif				// DELAY_H

//-----------------------------------------------------------------------------
