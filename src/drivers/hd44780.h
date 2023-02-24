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

// mode identifier and n-bit mask value
#define LCD_MODE4 15		// 4-bit interface
#define LCD_MODE8 255		// 8-bit interface

//-----------------------------------------------------------------------------

struct lcd_ctrl {
	int mode;		// 4 or 8 bit
	uint8_t rows, cols;	// number of character rows and columns
	void (*clk_hi)(void);	// clock high
	void (*clk_lo)(void);	// clock low
	void (*rs_hi)(void);	// register select high
	void (*rs_lo)(void);	// register select low
	void (*wr)(uint8_t val);	// write 4 or 8 bits
};

int lcd_init(struct lcd_ctrl *ctrl);
void lcd_test(struct lcd_ctrl *ctrl);
int lcd_cursor(struct lcd_ctrl *ctrl, uint8_t row, uint8_t col);
void lcd_clr(struct lcd_ctrl *ctrl);
void lcd_clr_row(struct lcd_ctrl *ctrl, uint8_t row);
void lcd_putc(struct lcd_ctrl *ctrl, uint8_t row, uint8_t col, char c);
void lcd_puts(struct lcd_ctrl *ctrl, uint8_t row, uint8_t col, char *s);

//-----------------------------------------------------------------------------

#endif				// HD44780_H

//-----------------------------------------------------------------------------
