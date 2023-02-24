//-----------------------------------------------------------------------------
/*

HD44780 LCD Controller Driver

https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller

Supports:

1 row x 16 col
1 row x 20 col
1 row x 24 col
1 row x 40 col

2 row x 16 col
2 row x 20 col
2 row x 24 col
2 row x 40 col

4 row x 16 col
4 row x 20 col

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "soc.h"
#include "hd44780.h"

//-----------------------------------------------------------------------------

#define LCD_FUNCTION_SET    (0x20 | (0 << 4 /*DL*/) | (1 << 3 /*N*/) | (0 << 2 /*F*/))
#define LCD_DISPLAY_ON      (0x08 | (1 << 2 /*D*/) | (0 << 1 /*C*/) | (0 << 0 /*B*/))
#define LCD_DISPLAY_CLEAR   (0x01)
#define LCD_ENTRY_MODE_SET  (0x04 | (1 << 1 /*I/D*/) | (0 << 0 /*S*/))

//#define LCD_DDRAM_ADR(x)    (0x80 | x)
//#define LCD_HOME            (0x02)
//#define LCD_ROW0            LCD_DDRAM_ADR(0)
//#define LCD_ROW1            LCD_DDRAM_ADR(0x40)

//-----------------------------------------------------------------------------

static void hd44780_wr4(struct hd44780_ctrl *ctrl, uint8_t val) {
	ctrl->clk_hi();
	ctrl->wr(val & 15);
	usDelay(5);
	ctrl->clk_lo();
	usDelay(50);
}

static void hd44780_wr(struct hd44780_ctrl *ctrl, uint8_t val) {
	if (ctrl->mode == HD44780_MODE4) {
		// write 2 x 4 bits
		hd44780_wr4(ctrl, val >> 4);
		hd44780_wr4(ctrl, val);
	} else {
		// write 1 x 8 bits
		ctrl->clk_hi();
		ctrl->wr(val);
		usDelay(5);
		ctrl->clk_lo();
		usDelay(50);
	}
}

// write 8 bits to the instruction/command register
static void hd44780_cmd(struct hd44780_ctrl *ctrl, uint8_t cmd) {
	ctrl->rs_lo();
	usDelay(5);
	hd44780_wr(ctrl, cmd);
}

// write 8 bits to the data register
static void hd44780_data(struct hd44780_ctrl *ctrl, uint8_t ch) {
	ctrl->rs_hi();
	usDelay(5);
	hd44780_wr(ctrl, ch);
}

//-----------------------------------------------------------------------------

// set the row/col cursor position
int hd44780_cursor(struct hd44780_ctrl *ctrl, uint8_t row, uint8_t col) {
	if (ctrl == NULL || row >= ctrl->rows || col >= ctrl->cols) {
		return -1;
	}
	switch (row) {
	case 0:
		if (ctrl->rows == 1) {
			if (col < (ctrl->cols >> 1)) {
				hd44780_cmd(ctrl, 0x80 + col);
			} else {
				hd44780_cmd(ctrl, 0xc0 + col - (ctrl->cols >> 1));
			}
		} else {
			hd44780_cmd(ctrl, 0x80 + col);
		}
		break;
	case 1:
		hd44780_cmd(ctrl, 0xc0 + col);
		break;
	case 2:
		hd44780_cmd(ctrl, 0x80 + ctrl->cols + col);
		break;
	case 3:
		hd44780_cmd(ctrl, 0xc0 + ctrl->cols + col);
		break;
	}
	return 0;
}

// clear the display
void hd44780_clr(struct hd44780_ctrl *ctrl) {
	hd44780_cmd(ctrl, LCD_DISPLAY_CLEAR);
	msDelay(2);
}

// clear a display row
void hd44780_clr_row(struct hd44780_ctrl *ctrl, uint8_t row) {
	// start of the row
	if (hd44780_cursor(ctrl, row, 0) < 0) {
		return;
	}
	// write out space characters
	for (uint8_t i = 0; i < ctrl->cols; i++) {
		hd44780_data(ctrl, ' ');
	}
	// back to the start of the row
	hd44780_cursor(ctrl, row, 0);
}

// display a character
void hd44780_putc(struct hd44780_ctrl *ctrl, uint8_t row, uint8_t col, char c) {
	if (hd44780_cursor(ctrl, row, col) < 0) {
		return;
	}
	hd44780_data(ctrl, c);
}

// display a string
void hd44780_puts(struct hd44780_ctrl *ctrl, uint8_t row, uint8_t col, char *s) {
	if (hd44780_cursor(ctrl, row, col) < 0 || s == NULL) {
		return;
	}
	uint8_t i = col;
	while ((i < ctrl->cols) && (*s != 0)) {
		hd44780_data(ctrl, *s++);
		i++;
	}
}

//-----------------------------------------------------------------------------

// 4-bit mode initialisation
static void hd44780_4bit_init(struct hd44780_ctrl *ctrl) {
	msDelay(20);
	hd44780_wr4(ctrl, 3);
	msDelay(10);
	hd44780_wr4(ctrl, 3);
	msDelay(1);
	hd44780_wr4(ctrl, 3);
	hd44780_wr4(ctrl, 2);

	hd44780_cmd(ctrl, LCD_FUNCTION_SET);
	hd44780_cmd(ctrl, LCD_DISPLAY_ON);
	hd44780_cmd(ctrl, LCD_DISPLAY_CLEAR);
	msDelay(2);

	hd44780_cmd(ctrl, LCD_ENTRY_MODE_SET);
}

// 8-bit mode initialisation
static void hd44780_8bit_init(struct hd44780_ctrl *ctrl) {

	msDelay(20);
	hd44780_wr(ctrl, 0x30);
	msDelay(10);
	hd44780_wr(ctrl, 0x30);
	msDelay(1);
	hd44780_wr(ctrl, 0x30);

	// TODO
}

int hd44780_init(struct hd44780_ctrl *ctrl) {
	if (ctrl == NULL) {
		return -1;
	}
	// initialise the display
	if (ctrl->mode == HD44780_MODE4) {
		hd44780_4bit_init(ctrl);
	} else {
		hd44780_8bit_init(ctrl);
	}
	return 0;
}

//-----------------------------------------------------------------------------
