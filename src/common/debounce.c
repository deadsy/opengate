//-----------------------------------------------------------------------------
/*

Switch Input Debounce

This code debounces up to 32 switch inputs (each switch state is a bit in a uint32_t).
To fit with a typical logic, off = 0 and on = 1.

The code samples the inputs in an ISR at an interval selectable by the user (typically 10-20ms)
and stores them in an array of DEBOUNCE_COUNT values.
When a debounced switch state is needed the samples in the array are ORed together.
This implies the code is fast to recognise an ON (single 1) and slow to recognise
an off (DEBOUNCE_COUNT 0's).

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "debounce.h"

//-----------------------------------------------------------------------------
// read and store the current switch state
// this is called periodically (10-20ms) from a timer ISR

void debounce_isr(struct debounce_ctrl *ctrl) {

	if (!ctrl->ready) {
		return;
	}
	// read and store the current input
	ctrl->sample[ctrl->idx] = ctrl->input();
	ctrl->idx = (ctrl->idx == DEBOUNCE_COUNT - 1) ? 0 : ctrl->idx + 1;

	// OR the samples for the new state
	uint32_t state = 0;
	for (int i = 0; i < DEBOUNCE_COUNT; i++) {
		state |= ctrl->sample[i];
	}

	if (state != ctrl->state) {
		uint32_t on_bits = ~ctrl->state & state;	// 0 to 1: switch is now ON
		uint32_t off_bits = ctrl->state & ~state;	// 1 to 0: switch is now OFF
		if (on_bits && ctrl->on) {
			ctrl->on(on_bits);
		}
		if (off_bits && ctrl->off) {
			ctrl->off(off_bits);
		}
		ctrl->state = state;
	}
}

//-----------------------------------------------------------------------------

int debounce_init(struct debounce_ctrl *ctrl) {
	if (ctrl == NULL) {
		return -1;
	}
	if (ctrl->input == NULL) {
		return -2;
	}
	for (int i = 0; i < DEBOUNCE_COUNT; i++) {
		ctrl->sample[i] = 0;
	}
	ctrl->state = 0;
	ctrl->idx = 0;
	ctrl->ready = 1;
	return 0;
}

//-----------------------------------------------------------------------------
