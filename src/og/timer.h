//-----------------------------------------------------------------------------
/*

Timer Processing

*/
//-----------------------------------------------------------------------------

#ifndef TIMER_H
#define TIMER_H

//-----------------------------------------------------------------------------

#include <inttypes.h>

//-----------------------------------------------------------------------------

// smallest timer tick
#define TIMER_SHIFT 6
#define TIMER_TICK (1 << TIMER_SHIFT)	// timer tick in millseconds

// timer states
#define TIMER_FREE 0		// must be == 0
#define TIMER_IDLE 1		// allocated and idle
#define TIMER_ACTIVE 2		// allocated and active

// timer types
#define TIMER_ONESHOT 0		// timeout once then go idle
#define TIMER_REPEAT 1		// timeout periodically

struct timer {
	uint8_t state;		// timer state
	uint8_t type;		// timer type
	uint32_t ticks;		// ticks remaining
	uint32_t timeout;	// timeout ticks
	void (*func)(void *arg);	// timeout function to call (optional)
	void *arg;		// user argument to timeout function
};

struct timer *timer_alloc(int type, uint32_t ms, void (*func)(void *arg), void *user);
int timer_free(struct timer *t);
int timer_start(struct timer *t);
int timer_stop(struct timer *t);
int timer_restart(struct timer *t);

int timer_init(void);
void timer_tick(void);

//-----------------------------------------------------------------------------

#endif				// TIMER_H

//-----------------------------------------------------------------------------
