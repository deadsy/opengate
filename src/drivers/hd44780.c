//-----------------------------------------------------------------------------
/*

HD44780 LCD Controller Driver

https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller

Note: These functions rely upon delays driven by the SysTick counter.
If they are called with the systick interrupt disabled they will not work correctly.
They should be called in a non-interrupt context.

Supports:
1x16, 1x20, 1x24, 1x40,
2x16, 2x20, 2x24, 2x40,
4x16, 4x20

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "soc.h"
#include "hd44780.h"

//-----------------------------------------------------------------------------

#define LCD_FUNCTION_SET(dl, n) (0x20 | ((dl) << 4) | ((n) << 3) | (0 << 2 /*F*/))
#define LCD_DISPLAY_ON          (0x08 | (1 << 2 /*D*/) | (0 << 1 /*C*/) | (0 << 0 /*B*/))
#define LCD_DISPLAY_OFF         (0x08 | (0 << 2 /*D*/) | (0 << 1 /*C*/) | (0 << 0 /*B*/))
#define LCD_DISPLAY_CLEAR       (0x01)
#define LCD_ENTRY_MODE_SET      (0x04 | (1 << 1 /*I/D*/) | (0 << 0 /*S*/))

//-----------------------------------------------------------------------------

static void lcd_wr4(struct lcd_ctrl *ctrl, uint8_t val) {
	ctrl->clk_hi();
	ctrl->wr(val & 15);
	usDelay(5);
	ctrl->clk_lo();
	usDelay(50);
}

static void lcd_wr(struct lcd_ctrl *ctrl, uint8_t val) {
	if (ctrl->mode == LCD_MODE4) {
		// write 2 x 4 bits
		lcd_wr4(ctrl, val >> 4);
		lcd_wr4(ctrl, val);
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
static void lcd_cmd(struct lcd_ctrl *ctrl, uint8_t cmd) {
	ctrl->rs_lo();
	usDelay(5);
	lcd_wr(ctrl, cmd);
}

// write 8 bits to the data register
static void lcd_data(struct lcd_ctrl *ctrl, uint8_t ch) {
	ctrl->rs_hi();
	usDelay(5);
	lcd_wr(ctrl, ch);
}

//-----------------------------------------------------------------------------

// set the row/col cursor position
int lcd_cursor(struct lcd_ctrl *ctrl, uint8_t row, uint8_t col) {
	if (row >= ctrl->rows || col >= ctrl->cols) {
		return -1;
	}
	switch (row) {
	case 0:
		if (ctrl->rows == 1) {
			if (col < (ctrl->cols >> 1)) {
				lcd_cmd(ctrl, 0x80 + col);
			} else {
				lcd_cmd(ctrl, 0xc0 + col - (ctrl->cols >> 1));
			}
		} else {
			lcd_cmd(ctrl, 0x80 + col);
		}
		break;
	case 1:
		lcd_cmd(ctrl, 0xc0 + col);
		break;
	case 2:
		lcd_cmd(ctrl, 0x80 + ctrl->cols + col);
		break;
	case 3:
		lcd_cmd(ctrl, 0xc0 + ctrl->cols + col);
		break;
	}
	return 0;
}

// clear the display
void lcd_clr(struct lcd_ctrl *ctrl) {
	lcd_cmd(ctrl, LCD_DISPLAY_CLEAR);
	msDelay(2);
}

// clear a display row
void lcd_clr_row(struct lcd_ctrl *ctrl, uint8_t row) {
	// start of the row
	if (lcd_cursor(ctrl, row, 0) != 0) {
		return;
	}
	// write out space characters
	for (uint8_t i = 0; i < ctrl->cols; i++) {
		lcd_data(ctrl, ' ');
	}
	// back to the start of the row
	lcd_cursor(ctrl, row, 0);
}

// display a character
void lcd_putc(struct lcd_ctrl *ctrl, uint8_t row, uint8_t col, char c) {
	if (lcd_cursor(ctrl, row, col) != 0) {
		return;
	}
	lcd_data(ctrl, c);
}

// display a string
void lcd_puts(struct lcd_ctrl *ctrl, uint8_t row, uint8_t col, char *s) {
	if (lcd_cursor(ctrl, row, col) != 0 || s == NULL) {
		return;
	}
	uint8_t i = col;
	while ((i < ctrl->cols) && (*s != 0)) {
		lcd_data(ctrl, *s++);
		i++;
	}
}

//-----------------------------------------------------------------------------

// display an lcd test pattern over all rows and columns.
void lcd_test(struct lcd_ctrl *ctrl) {
	lcd_clr(ctrl);
	int k = 0;
	for (int i = 0; i < ctrl->rows; i++) {
		for (int j = 0; j < ctrl->cols; j++) {
			lcd_putc(ctrl, i, j, 'A' + (k % 26));
			k++;
		}
	}
}

//-----------------------------------------------------------------------------

// 4-bit mode initialisation
static void lcd_4bit_init(struct lcd_ctrl *ctrl) {
	msDelay(16);
	lcd_wr4(ctrl, 3);
	msDelay(5);
	lcd_wr4(ctrl, 3);
	msDelay(1);
	lcd_wr4(ctrl, 3);
	lcd_wr4(ctrl, 2);

	lcd_cmd(ctrl, (ctrl->rows == 1) ? LCD_FUNCTION_SET(0, 0) : LCD_FUNCTION_SET(0, 1));
	lcd_cmd(ctrl, LCD_DISPLAY_ON);
	lcd_clr(ctrl);
	lcd_cmd(ctrl, LCD_ENTRY_MODE_SET);
}

// 8-bit mode initialisation
static void lcd_8bit_init(struct lcd_ctrl *ctrl) {
	msDelay(16);
	lcd_wr(ctrl, 0x30);
	msDelay(5);
	lcd_wr(ctrl, 0x30);
	msDelay(1);
	lcd_wr(ctrl, 0x30);

	lcd_cmd(ctrl, (ctrl->rows == 1) ? LCD_FUNCTION_SET(1, 0) : LCD_FUNCTION_SET(1, 1));
	lcd_cmd(ctrl, LCD_DISPLAY_ON);
	lcd_clr(ctrl);
	lcd_cmd(ctrl, LCD_ENTRY_MODE_SET);
}

int lcd_init(struct lcd_ctrl *ctrl) {
	// initialise the display
	if (ctrl->mode == LCD_MODE4) {
		lcd_4bit_init(ctrl);
	} else {
		lcd_8bit_init(ctrl);
	}
	return 0;
}

//-----------------------------------------------------------------------------
