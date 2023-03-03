//-----------------------------------------------------------------------------
/*

Event Processing

*/
//-----------------------------------------------------------------------------

#ifndef EVENT_H
#define EVENT_H

//-----------------------------------------------------------------------------

#include <inttypes.h>

//-----------------------------------------------------------------------------

// event type in the upper 8 bits
#define EVENT_TYPE(x) ((x) & 0xff000000U)
#define EVENT_TYPE_KEY_DN (1U << 24)
#define EVENT_TYPE_KEY_UP (2U << 24)

// key number in the lower 8 bits
#define EVENT_KEY(x) ((x) & 0xffU)

struct event {
	uint32_t type;		// the event type
	void *ptr;		// pointer to event data (or data itself)
};

int event_init(void);
int event_rd(struct event *event);
int event_wr(uint32_t type, void *ptr);

//-----------------------------------------------------------------------------

#endif				// EVENT_H

//-----------------------------------------------------------------------------
