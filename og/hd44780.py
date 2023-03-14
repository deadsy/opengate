# driver for a text lcd with a hd44780 chip

# provide board pin numbers as a tuple in order:
# Rs, E, D4, D5, D6, D7 (4 bit mode)
# Rs, E, D0, D1, D2, D3, D4, D5, D6, D7 (8 bit mode)

from machine import Pin
import utime as time

LCD_DISPLAY_ON = 0x08 | (1 << 2) | (0 << 1) | (0 << 0)
LCD_DISPLAY_OFF = 0x08 | (0 << 2) | (0 << 1) | (0 << 0)
LCD_DISPLAY_CLEAR = 0x01
LCD_ENTRY_MODE_SET = 0x04 | (1 << 1) | (0 << 0)


def LCD_FUNCTION_SET(dl, n):
    return 0x20 | ((dl) << 4) | ((n) << 3) | (0 << 2)


class LCD:
    def __init__(self, mode, pinlist, rows, cols):
        assert rows in (1, 2, 4), "bad rows"
        assert cols in (16, 20, 24, 40), "bad cols"
        assert mode in (4, 8), "bad mode"
        assert len(pinlist) == 2 + mode, "bad pinlist"
        self.mode = mode
        self.cols = cols
        self.rows = rows
        # init hardware pins
        self.LCD_RS = Pin(pinlist[0], Pin.OUT)
        self.LCD_E = Pin(pinlist[1], Pin.OUT)
        self.datapins = [Pin(pin_name, Pin.OUT) for pin_name in pinlist[2:]]
        # initialise the hd44780 chip
        if self.mode == 4:
            # 4 bit mode
            time.sleep_ms(16)
            self._wr(3)
            time.sleep_ms(5)
            self._wr(3)
            time.sleep_ms(1)
            self._wr(3)
            self._wr(2)
        else:
            # 8 bit mode
            time.sleep_ms(16)
            self._wr(0x30)
            time.sleep_ms(5)
            self._wr(0x30)
            time.sleep_ms(1)
            self._wr(0x30)
        self.wr_cmd(LCD_FUNCTION_SET((1, 0)[self.mode == 4], (1, 0)[self.rows == 1]))
        self.wr_cmd(LCD_DISPLAY_ON)
        self.clr()
        self.wr_cmd(LCD_ENTRY_MODE_SET)

    def _wr(self, val):  # write 4 or 8 bits
        self.LCD_E.on()
        for pin in self.datapins:
            pin.value(val & 1)
            val >>= 1
        time.sleep_us(5)
        self.LCD_E.off()
        time.sleep_us(50)

    def wr(self, val):  # write 8 bits
        if self.mode == 4:
            # write 2 x 4 bits
            self._wr(val >> 4)
            self._wr(val)
        else:
            # write 1 x 8 bits
            self._wr(val)

    def wr_cmd(self, cmd):  # write command
        self.LCD_RS.off()
        time.sleep_us(5)
        self.wr(cmd)

    def wr_data(self, val):  # write data
        self.LCD_RS.on()
        time.sleep_us(5)
        self.wr(val)

    def set_cursor(self, row, col):  # set the cursor position
        assert row < self.rows and row >= 0, "bad row"
        assert col < self.cols and col >= 0, "bad col"
        if row == 0:
            self.wr_cmd(0x80 + col)
        elif row == 1:
            self.wr_cmd(0xC0 + col)
        elif row == 2:
            self.wr_cmd(0x80 + self.cols + col)
        elif row == 3:
            self.wr_cmd(0xC0 + self.cols + col)

    def symbol(self, id, data): # user defined symbol
        self.wr_cmd(0x40 + ((id & 7) << 3))
        for i in range(8):
            self.wr_data(data[i])

    def clr(self):  # clear the display
        self.wr_cmd(LCD_DISPLAY_CLEAR)
        time.sleep_ms(2)

    def on(self):  # turn the display on
        self.wr_cmd(LCD_DISPLAY_ON)

    def off(self):  # turn the display off
        self.wr_cmd(LCD_DISPLAY_OFF)

    def clr_row(self, row):  # clear a display row
        self.set_cursor(row, 0)
        for i in range(self.cols):
            self.wr_data(" ")
        self.set_cursor(row, 0)

    def putc(self, row, col, c):  # display a character
        self.set_cursor(row, col)
        self.wr_data(ord(c))

    def puts(self, row, col, s):  # display a string
        self.set_cursor(row, col)
        n = min(len(s), self.cols - col)
        for i in range(n):
            self.wr_data(ord(s[i]))

    def test(self):  # show a test pattern
        self.clr()
        k = 0
        for i in range(self.rows):
            for j in range(self.cols):
                self.putc(i, j, chr(ord("A") + (k % 26)))
                k += 1
