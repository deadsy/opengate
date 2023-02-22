//-----------------------------------------------------------------------------
/*

Key Matrix Scanning

*/
//-----------------------------------------------------------------------------

#include <stddef.h>
#include <string.h>

#include "keyscan.h"

//-----------------------------------------------------------------------------

// key state - 8 bits
// the top 3 bits indicate the key state
// the bottom 5 bits are a debounce counter

#define KEY_STATE_UNKNOWN    (0 << 5)
#define KEY_STATE_WAIT4_DOWN (1 << 5)
#define KEY_STATE_DOWN       (2 << 5)
#define KEY_STATE_WAIT4_UP   (3 << 5)
#define KEY_STATE_UP         (4 << 5)

#define KEY_COUNT(x) (ctrl->state[x] & (31 << 0))
#define KEY_STATE(x) (ctrl->state[x] & (7 << 5))

// debounce counts
#define DEBOUNCE_COUNT_DOWN 1
#define DEBOUNCE_COUNT_UP 3

//-----------------------------------------------------------------------------

void keyscan_isr(struct keyscan_ctrl *ctrl) {

	if (!ctrl->ready) {
		return;
	}
	// read the column lines
	uint32_t col = ctrl->read_col();
	int key = ctrl->row;

	for (int i = 0; i < ctrl->cols; i++) {
		int down = col & 1;
		switch (KEY_STATE(key)) {
		case KEY_STATE_WAIT4_DOWN:{
				// wait for n successive key down conditions
				if (!down) {
					ctrl->state[key] = KEY_STATE_UP;
				} else {
					int n = KEY_COUNT(key);
					if (n >= DEBOUNCE_COUNT_DOWN) {
						ctrl->state[key] = KEY_STATE_DOWN;
						if (ctrl->key_dn) {
							ctrl->key_dn(key);
						}
					} else {
						ctrl->state[key] = KEY_STATE_WAIT4_DOWN | (n + 1);
					}
				}
				break;
			}
		case KEY_STATE_DOWN:{
				// the key is down
				if (!down) {
					ctrl->state[key] = KEY_STATE_WAIT4_UP;
				}
				break;
			}
		case KEY_STATE_WAIT4_UP:{
				// wait for n successive key up conditions
				if (down) {
					ctrl->state[key] = KEY_STATE_DOWN;
				} else {
					int n = KEY_COUNT(key);
					if (n >= DEBOUNCE_COUNT_UP) {
						ctrl->state[key] = KEY_STATE_UP;
						if (ctrl->key_up) {
							ctrl->key_up(key);
						}
					} else {
						ctrl->state[key] = KEY_STATE_WAIT4_UP | (n + 1);
					}
				}
				break;
			}
		case KEY_STATE_UP:{
				// the key is up
				if (down) {
					ctrl->state[key] = KEY_STATE_WAIT4_DOWN;
				}
				break;
			}
		default:{
				ctrl->state[key] = down ? KEY_STATE_DOWN : KEY_STATE_UP;
				break;
			}
		}
		// next column
		col >>= 1;
		key += ctrl->rows;
	}
	// select the next row
	ctrl->clr_row(ctrl->row);
	ctrl->row = (ctrl->row == ctrl->rows - 1) ? 0 : ctrl->row + 1;
	ctrl->set_row(ctrl->row);
}

//-----------------------------------------------------------------------------

int keyscan_init(struct keyscan_ctrl *ctrl) {
	if (ctrl == NULL) {
		return -1;
	}
	if (ctrl->rows == 0 || ctrl->cols == 0) {
		return -2;
	}
	if (ctrl->state == NULL) {
		return -3;
	}
	if (ctrl->clr_row == NULL || ctrl->set_row == NULL) {
		return -4;
	}
	if (ctrl->read_col == NULL) {
		return -5;
	}
	memset(ctrl->state, 0, ctrl->cols * ctrl->rows);
	ctrl->row = 0;
	ctrl->set_row(0);
	ctrl->ready = 1;
	return 0;
}

//-----------------------------------------------------------------------------
