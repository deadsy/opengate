//-----------------------------------------------------------------------------
/*

Polled Delays

*/
//-----------------------------------------------------------------------------

#include "soc.h"

//-----------------------------------------------------------------------------

// global 1ms tick counter updated by SysTick_Handler.
volatile uint32_t globalTick;

//-----------------------------------------------------------------------------

#define TICK_CLK 4		// systick clock in MHz

static uint64_t get_current_usecs(void) {
	return (getTick() * 1000) + 1000 - (SysTick->VAL / TICK_CLK);
}

//-----------------------------------------------------------------------------

// millisecond delay
void msDelay(uint32_t msecs) {
	uint32_t start = getTick();
	uint32_t wait = msecs;
	// actual delay is between msecs and msecs+1 milliseconds.
	if (wait < UINT32_MAX) {
		wait++;
	}
	// delay (wrap around safe)
	while ((getTick() - start) < wait) ;
}

// microsecond delay
void usDelay(uint32_t usecs) {
	uint64_t start = get_current_usecs();
	// delay (wrap around safe)
	while ((get_current_usecs() - start) < usecs) ;
}

//-----------------------------------------------------------------------------
