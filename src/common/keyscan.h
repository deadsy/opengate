//-----------------------------------------------------------------------------
/*

Key Matrix Scanning

*/
//-----------------------------------------------------------------------------

#ifndef KEYSCAN_H
#define KEYSCAN_H

//-----------------------------------------------------------------------------

#include <inttypes.h>

//-----------------------------------------------------------------------------

struct keyscan_ctrl {
	int ready;		// initialised
	int rows;		// number of matrix rows
	int cols;		// number of matrix cols
	int row;		// current row
	uint8_t *state;		// row x col bytes for key state
	void (*key_dn)(int key);	// provided key down handler (optional)
	void (*key_up)(int key);	// provided key up handler (optional)
	void (*clr_row)(int row);	// provided function to clear a row line
	void (*set_row)(int row);	// provided function to set a row line
	 uint32_t(*read_col) (void);	// provided function to read a column
};

int keyscan_init(struct keyscan_ctrl *ctrl);
void keyscan_isr(struct keyscan_ctrl *ctrl);

//-----------------------------------------------------------------------------

#endif				// KEYSCAN_H

//-----------------------------------------------------------------------------
