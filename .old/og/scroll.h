//-----------------------------------------------------------------------------
/*

String Scroller

left/right wrapped scrolling on a limited width display

*/
//-----------------------------------------------------------------------------

#ifndef SCROLL_H
#define SCROLL_H

//-----------------------------------------------------------------------------

#define SCROLL_LEFT 0
#define SCROLL_RIGHT 1

struct scroll_ctrl {
	char *s;		// string to scroll
	int width;		// display width in characters
	int dirn;		// direction of scroll (left/right)
	int len;		// string length
	int n;			// scroll position (within string)
	void (*puts)(char *s, int col);	// put string function
};

int scroll_init(struct scroll_ctrl *ctrl);
void scroll_update(struct scroll_ctrl *ctrl);

//-----------------------------------------------------------------------------

#endif				// SCROLL_H

//-----------------------------------------------------------------------------
