# keypad matrix scanner

from machine import Pin
import utime as time
import uasyncio as asyncio

# time between row polls (ms)
ROW_POLL_TIME = 10

# row select active high/low
ROW_SELECT_HIGH = True

# input pull up/down
INPUT_PULL = (Pin.PULL_UP, Pin.PULL_DOWN)[ROW_SELECT_HIGH]

# debounce counts
DEBOUNCE_DN_COUNT = 0
DEBOUNCE_UP_COUNT = 1

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
    def __init__(self, event_out, rowpins, colpins):
        assert len(rowpins) >= 1, "bad rows"
        assert len(colpins) >= 1, "bad cols"
        self.queue = event_out
        self.row_io = [Pin(x, Pin.OUT) for x in rowpins]
        self.col_io = [Pin(x, Pin.IN, INPUT_PULL) for x in colpins]
        self.rows = len(rowpins)
        self.cols = len(colpins)
        self.state = [keystate() for i in range(self.rows * self.cols)]
        # init rows
        for i in range(self.rows):
            self.deselect_row(i)
        self.row = 0
        self.select_row(0)

    def select_row(self, i):
        self.row_io[i].value(ROW_SELECT_HIGH)

    def deselect_row(self, i):
        self.row_io[i].value(not ROW_SELECT_HIGH)

    def read_col(self):
        return [(x.value() == int(ROW_SELECT_HIGH)) for x in self.col_io]

    async def scan_row(self):
        col = self.read_col()
        key = self.row
        for i in range(self.cols):
            down = col[i]
            ks = self.state[key]
            if ks.state == KEY_STATE_WAIT4_DOWN:
                # wait for n successive key down conditions
                if not down:
                    ks.set(KEY_STATE_UP)
                else:
                    if ks.count >= DEBOUNCE_DN_COUNT:
                        ks.set(KEY_STATE_DOWN)
                        await self.queue.put((True, key))
                    else:
                        ks.inc()
            elif ks.state == KEY_STATE_DOWN:
                if not down:
                    ks.set(KEY_STATE_WAIT4_UP)
            elif ks.state == KEY_STATE_WAIT4_UP:
                # wait for n successive key up conditions
                if down:
                    ks.set(KEY_STATE_DOWN)
                else:
                    if ks.count >= DEBOUNCE_UP_COUNT:
                        ks.set(KEY_STATE_UP)
                        await self.queue.put((False, key))
                    else:
                        ks.inc()
                pass
            elif ks.state == KEY_STATE_UP:
                if down:
                    ks.set(KEY_STATE_WAIT4_DOWN)
            else:
                ks.set((KEY_STATE_UP, KEY_STATE_DOWN)[down])
            # next column
            key += self.rows

    async def scan(self):  # scan the key matrix
        while True:
            await self.scan_row()
            # next row
            self.deselect_row(self.row)
            self.row = (self.row + 1) % self.rows
            self.select_row(self.row)
            await asyncio.sleep_ms(ROW_POLL_TIME)
