# open gate

import hd44780
import scroll
import relay
import sim7600

import utime as time
from machine import UART

# lcd display
lcd = hd44780.LCD(4, (10, 11, 6, 7, 8, 9), 2, 16)

# relay board
r0 = relay.relay("r0", 14)
r1 = relay.relay("r1", 16)

# sim7600 modem
uart = UART(0)
uart.init(baudrate=115200) # tx=0, rx=1
modem = sim7600.modem(uart, pwr=11, fc=6)
print("%s" % modem)

def main():
    lcd.symbol(0, (0, 0x0a,0x1f,0x1f,0xe,0x04,0,0))
    s0 = scroll.scroll("\x00Arbeit Macht Frei...", 16, lambda s: lcd.puts(0, 0, s))
    s1 = scroll.scroll("\x00\x00\x00Arby's Makes Fries...", 16, lambda s: lcd.puts(1, 0, s))
    while True:
        s0.update()
        s1.update()
        time.sleep_ms(400)


main()
