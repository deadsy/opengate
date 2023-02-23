//-----------------------------------------------------------------------------
/*

Polled Delays

*/
//-----------------------------------------------------------------------------

#include "soc.h"

//-----------------------------------------------------------------------------

volatile uint32_t globalTick;

//-----------------------------------------------------------------------------

#define TICK_CLK 4		// systick count down clock in MHz

static uint64_t get_current_usecs(void) {
	return (getTick() * 1000) + 1000 - (SysTick->VAL / TICK_CLK);
}

//-----------------------------------------------------------------------------

void msDelay(uint32_t msecs) {
	uint32_t start = getTick();
	uint32_t wait = msecs;
	// Add a period to guarantee minimum wait
	if (wait < UINT32_MAX) {
		wait++;
	}
	while ((getTick() - start) < wait) ;
}

void usDelay(uint32_t usecs) {
	uint64_t timeout = get_current_usecs() + usecs;
	while (get_current_usecs() < timeout) ;
}

//-----------------------------------------------------------------------------
