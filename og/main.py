# open gate

import hd44780
import scroll
import relay
import sim7600
import gsm

import utime as time
from machine import UART
from machine import Pin

# lcd display
lcd = hd44780.LCD(4, (10, 11, 6, 7, 8, 9), 2, 16)

# relay board
r0 = relay.relay("r0", 14)
r1 = relay.relay("r1", 16)
r2 = relay.relay("r2", 15)

# sim7600 modem
uart = UART(0)
uart.init(baudrate=115200, timeout=20)  # tx=0, rx=1
modem = sim7600.modem(uart, pwr=11)

# pico led
led = Pin(25, Pin.OUT)


def relay_test():
    rstate = 0
    while True:
        if rstate == 0:
            r0.toggle()
        elif rstate == 1:
            r1.toggle()
        elif rstate == 2:
            r2.toggle()
        rstate = (rstate + 1) % 3
        time.sleep_ms(500)


def lcd_test():
    lcd.symbol(0, (0, 0x0A, 0x1F, 0x1F, 0xE, 0x04, 0, 0))
    s0 = scroll.scroll("\x00Arbeit Macht Frei...", 16, lambda s: lcd.puts(0, 0, s))
    s1 = scroll.scroll("\x00\x00\x00Arby's Makes Fries...", 16, lambda s: lcd.puts(1, 0, s))
    while True:
        s0.update()
        s1.update()
        time.sleep_ms(400)


def dump(s):
    print(" ".join(["%02x" % c for c in s]))


def main():
    modem.setup()
    print(modem)
    gsm.init()

    # modem.set_sms_format(1)
    # modem.sms_send_txt("+14087102537", "text message from opengate!")

    modem.set_sms_format(0)
    modem.sms_send_pdu("+14087102537", "[]{} \xf1\xf2 pdu message from opengate!")

    # modem.set_call_voice_device(1)
    # modem.set_call_loudspeaker_level(5)
    # modem.make_call("+14087102537")

    print("done")

    while True:
        led.toggle()
        time.sleep_ms(1000)


main()
