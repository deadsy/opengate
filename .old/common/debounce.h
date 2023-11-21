//-----------------------------------------------------------------------------
/*

Switch Input Debouncing

*/
//-----------------------------------------------------------------------------

#ifndef DEBOUNCE_H
#define DEBOUNCE_H

//-----------------------------------------------------------------------------

#include <inttypes.h>

//-----------------------------------------------------------------------------

#define DEBOUNCE_COUNT 4

struct debounce_ctrl {
	int ready;		// initialised
	uint32_t sample[DEBOUNCE_COUNT];
	uint32_t state;
	int idx;
	void (*on)(uint32_t bits);	// provided switch on handler (optional)
	void (*off)(uint32_t bits);	// provided switch off handler (optional)
	 uint32_t(*input) (void);	// read button/switch input
};

int debounce_init(struct debounce_ctrl *ctrl);
void debounce_isr(struct debounce_ctrl *ctrl);

//-----------------------------------------------------------------------------

#endif				// DEBOUNCE_H

//-----------------------------------------------------------------------------
