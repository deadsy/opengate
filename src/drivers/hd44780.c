//-----------------------------------------------------------------------------
/*

HD44780 LCD Controller Driver

https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller

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

static void hd44780_wr(struct hd44780_ctrl *ctrl, uint8_t val) {
	ctrl->clk_hi();
	ctrl->wr4(val);
	usDelay(5);
	ctrl->clk_lo();
	usDelay(50);
}

// write 8 bits to the instruction/command register
static void hd44780_cmd(struct hd44780_ctrl *ctrl, uint8_t cmd) {
	ctrl->rs_lo();
	usDelay(5);
	hd44780_wr(ctrl, cmd >> 4);
	hd44780_wr(ctrl, cmd);
}

// write 8 bits to the data register
static void hd44780_char(struct hd44780_ctrl *ctrl, uint8_t ch) {
	ctrl->rs_hi();
	usDelay(5);
	hd44780_wr(ctrl, ch >> 4);
	hd44780_wr(ctrl, ch);
}

//-----------------------------------------------------------------------------
// stdio compatible putc

void hd44780_foo(struct hd44780_ctrl *ctrl) {
	hd44780_char(ctrl, 0);
}

//-----------------------------------------------------------------------------

int hd44780_init(struct hd44780_ctrl *ctrl) {
	if (ctrl == NULL) {
		return -1;
	}
	if (ctrl->clk_hi == NULL || ctrl->clk_lo == NULL) {
		return -2;
	}
	if (ctrl->rs_hi == NULL || ctrl->rs_lo == NULL) {
		return -3;
	}
	if (ctrl->wr4 == NULL) {
		return -4;
	}
	// 4 bit setup as per hd44780 datasheet
	msDelay(20);
	hd44780_wr(ctrl, 3);
	msDelay(10);
	hd44780_wr(ctrl, 3);
	msDelay(1);
	hd44780_wr(ctrl, 3);
	hd44780_wr(ctrl, 2);

	hd44780_cmd(ctrl, LCD_FUNCTION_SET);
	hd44780_cmd(ctrl, LCD_DISPLAY_ON);
	hd44780_cmd(ctrl, LCD_DISPLAY_CLEAR);
	msDelay(2);
	hd44780_cmd(ctrl, LCD_ENTRY_MODE_SET);

	memset(ctrl->shadow, 0, ctrl->rows * ctrl->cols);
	return 0;
}

//-----------------------------------------------------------------------------
