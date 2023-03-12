# keypad matrix scanner

from machine import Pin
import utime as time

ROW_POLL_TIME = 16

# key state
KEY_STATE_UNKNOWN = 0
KEY_STATE_WAIT4_DOWN = 1
KEY_STATE_DOWN = 2
KEY_STATE_WAIT4_UP = 3
KEY_STATE_UP = 4


class keystate:
    def __init__(self):
        self.state = KEY_STATE_UNKNOWN
        self.count = 0

    def set(self, x):  # set the key state
        self.state = x
        self.count = 0

    def inc(self):  # increment the key count
        self.count += 1


class matrix:

    def __init__(self, rowpins, colpins):
        assert len(rowpins) >= 1, "bad rows"
        assert len(colpins) >= 1, "bad cols"
        self.row_io = [Pin(x, Pin.OUT) for x in rowpins]
        self.col_io = [Pin(x, Pin.IN) for x in colpins]
        self.rows = len(rowpins)
        self.cols = len(colpins)
        self.state = [keystate(),] * (self.rows * self.cols)
        self.debounce_dn = 1
        self.debounce_up = 2
        # init rows
        for i in range(self.rows):
            self.row_io[i].low()
        self.row = 0
        self.row_io[0].high()

    def rd_col(self):  # read the column pins
        return [(x.value() == 1) for x in self.col_io]

    def scan_row(self):  # scan the row
        col = self.rd_col()
        key = self.row
        for i in range(self.cols):
            down = col[i]
            ks = self.state[key]
            if ks.state == KEY_STATE_WAIT4_DOWN:
                # wait for n successive key down conditions
                if not down:
                    ks.set(KEY_STATE_UP)
                else:
                    if ks.count >= self.debounce_dn:
                        ks.set(KEY_STATE_DOWN)
                        # TODO emit down
                    else:
                        ks.inc()
            if ks.state == KEY_STATE_DOWN:
                if not down:
                    ks.set(KEY_STATE_WAIT4_UP)
            if ks.state == KEY_STATE_WAIT4_UP:
                # wait for n successive key up conditions
                if down:
                    ks.set(KEY_STATE_DOWN)
                else:
                    if ks.count >= self.debounce_up:
                        ks.set(KEY_STATE_UP)
                        # TODO emit up
                    else:
                        ks.inc()
            if ks.state == KEY_STATE_UP:
                if down:
                    ks.set(KEY_STATE_WAIT4_DOWN)
            else:
                ks.set((KEY_STATE_UP, KEY_STATE_DOWN)[down])
            # next column
            key += self.rows

    def scan(self):  # scan the key matrix
        while True:
            self.scan_row()
            # next row
            self.row_io[self.row].low()
            self.row = (self.row + 1) % self.rows
            self.row_io[self.row].high()
            time.sleep_ms(ROW_POLL_TIME)
