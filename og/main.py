# open gate

import hd44780
import scroll
import relay
import sim7600

import utime as time
from machine import UART
from machine import Pin

# lcd display
lcd = hd44780.LCD(4, (10, 11, 6, 7, 8, 9), 2, 16)

# relay board
r0 = relay.relay("r0", 14)
r1 = relay.relay("r1", 16)

# sim7600 modem
uart = UART(0)
uart.init(baudrate=115200, timeout=20)  # tx=0, rx=1
modem = sim7600.modem(uart, pwr=11)


def main():
    led_io = Pin(25, Pin.OUT)
    modem.setup()
    print(modem)

    # modem.gps(False)
    # modem.gps(True)

    while True:
        led_io.toggle()
        # print(modem.gps_info())
        time.sleep_ms(1000)

    # lcd.symbol(0, (0, 0x0A, 0x1F, 0x1F, 0xE, 0x04, 0, 0))
    # s0 = scroll.scroll("\x00Arbeit Macht Frei...", 16, lambda s: lcd.puts(0, 0, s))
    # s1 = scroll.scroll("\x00\x00\x00Arby's Makes Fries...", 16, lambda s: lcd.puts(1, 0, s))
    # while True:
    #    s0.update()
    #    s1.update()
    #    time.sleep_ms(400)


main()
