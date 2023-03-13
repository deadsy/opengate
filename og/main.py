# open gate

import hd44780
import scroll
import utime as time

lcd = hd44780.LCD(4, (10, 11, 6, 7, 8, 9), 2, 16)

def main():
    s0 = scroll.scroll("Arbeit Macht Frei...", 16, lambda s: lcd.puts(0, 0, s))
    s1 = scroll.scroll("Arby's Makes Fries...", 16, lambda s: lcd.puts(1, 0, s))
    while True:
        s0.update()
        s1.update()
        time.sleep_ms(400)


main()
