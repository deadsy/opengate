//-----------------------------------------------------------------------------
/*

Timer Processing

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "timer.h"

//-----------------------------------------------------------------------------

#define NUM_TIMERS 16

// timer array
static struct timer timers[NUM_TIMERS];

//-----------------------------------------------------------------------------

// allocate (and start) a timer
struct timer *timer_alloc(int type, uint32_t ms, void (*func)(void *arg), void *arg) {
	int idx;
	// find a free timer
	for (idx = 0; idx < NUM_TIMERS; idx++) {
		if (timers[idx].state == TIMER_FREE) {
			break;
		}
	}
	if (idx == NUM_TIMERS) {
		// no free timers
		return NULL;
	}
	if (type != TIMER_ONESHOT && type != TIMER_REPEAT) {
		// bad timer type
		return NULL;
	}
	// setup the timer
	int ticks = ms >> TIMER_SHIFT;
	struct timer *t = &timers[idx];
	t->state = TIMER_ACTIVE;
	t->type = type;
	t->ticks = ticks;
	t->timeout = ticks;
	t->func = func;
	t->arg = arg;
	return t;
}

// free a timer
int timer_free(struct timer *t) {
	t->state = TIMER_FREE;
	return 0;
}

// start a timer
int timer_start(struct timer *t) {
	if (t->state == TIMER_FREE) {
		return -1;
	}
	t->state = TIMER_ACTIVE;
	return 0;
}

// stop a timer
int timer_stop(struct timer *t) {
	if (t->state == TIMER_FREE) {
		return -1;
	}
	t->state = TIMER_IDLE;
	return 0;
}

// restart a timer
int timer_restart(struct timer *t) {
	if (t->state == TIMER_FREE) {
		return -1;
	}
	t->state = TIMER_ACTIVE;
	t->ticks = t->timeout;
	return 0;
}

// call the timeout functions for expired timers
void timer_tick(void) {
	for (int idx = 0; idx < NUM_TIMERS; idx++) {
		struct timer *t = &timers[idx];
		// is this timer active?
		if (t->state != TIMER_ACTIVE) {
			continue;
		}
		t->ticks--;
		if (t->ticks == 0) {
			if (t->type == TIMER_ONESHOT) {
				// go to idle
				t->state = TIMER_IDLE;
			} else if (t->type == TIMER_REPEAT) {
				// auto restart
				t->ticks = t->timeout;
			}
			// call the timeout function
			if (t->func != NULL) {
				t->func(t->arg);
			}
		}
	}
}

//-----------------------------------------------------------------------------

// initialise timers
int timer_init(void) {
	memset(timers, 0, sizeof(timers));
	return 0;
}

//-----------------------------------------------------------------------------
