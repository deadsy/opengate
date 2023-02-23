//-----------------------------------------------------------------------------
/*

HD44780 LCD Controller Driver

*/
//-----------------------------------------------------------------------------

#ifndef HD44780_H
#define HD44780_H

//-----------------------------------------------------------------------------

#include <inttypes.h>

//-----------------------------------------------------------------------------

#define HD44780_MODE4 4		// 4-bit interface
#define HD44780_MODE8 8		// 8-bit interface

//-----------------------------------------------------------------------------

struct hd44780_ctrl {
	int mode;		// 4 or 8 bit
	int rows, cols;		// number of character rows and columns
	uint8_t *shadow;	// rows x cols shadow lcd contents
	int row, col;		// current row/col position
	void (*clk_hi)(void);	// clock high
	void (*clk_lo)(void);	// clock low
	void (*rs_hi)(void);	// register select high
	void (*rs_lo)(void);	// register select low
	void (*wr)(uint8_t val);	// write 4 or 8 bits
};

int hd44780_init(struct hd44780_ctrl *ctrl);

//-----------------------------------------------------------------------------

#endif				// HD44780_H

//-----------------------------------------------------------------------------
