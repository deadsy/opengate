//-----------------------------------------------------------------------------
/*

String Scroller

left/right wrapped scrolling on a limited width display

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "scroll.h"

//-----------------------------------------------------------------------------

static inline int min(int a, int b) {
	return (a < b) ? a : b;
}

//-----------------------------------------------------------------------------

static void scroll_output(struct scroll_ctrl *ctrl) {
	char buf[32];
	int k = ctrl->n;
	int col = 0;
	while (col < ctrl->width) {
		int n = min(sizeof(buf) - 1, ctrl->width - col);
		for (int i = 0; i < n; i++) {
			buf[i] = ctrl->s[k++];
			if (k > ctrl->len) {
				k = 0;
			}
		}
		buf[n] = 0;
		ctrl->puts(buf, col);
		col += n;
	}
}

void scroll_update(struct scroll_ctrl *ctrl) {
	if (ctrl->dirn == SCROLL_RIGHT) {
		// decrement with wrap
		ctrl->n--;
		if (ctrl->n < 0) {
			ctrl->n = ctrl->len;
		}
	} else {
		// increment with wrap
		ctrl->n++;
		if (ctrl->n > ctrl->len) {
			ctrl->n = 0;
		}
	}
	scroll_output(ctrl);
}

int scroll_init(struct scroll_ctrl *ctrl) {
	ctrl->len = strlen(ctrl->s) - 1;
	ctrl->n = 0;
	scroll_output(ctrl);
	return 0;
}

//-----------------------------------------------------------------------------
