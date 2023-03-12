# open gate

import hd44780
import scroll
import utime as time

lcd = hd44780.LCD(4, (10, 11, 6, 7, 8, 9), 2, 16)

def puts0(s):
    lcd.puts(0, 0, s)

def puts1(s):
    lcd.puts(1, 0, s)

def main():
    s0 = scroll.scroll("Arbeit Macht Frei...", False, 16, puts0)
    s1 = scroll.scroll("Arby's Makes Fries...", False, 16, puts1)
    for i in range(100):
        s0.update()
        s1.update()
        time.sleep_ms(400)

main()
