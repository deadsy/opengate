# open gate

import hd44780
import keypad


def main():
    lcd = hd44780.LCD(4, (21, 22, 24, 25, 26, 27), 2, 16)
    lcd.test()
    lcd.puts(0, 0, "OpenGate")

    key = keypad.matrix((4,5,6), (7,8,9))
    key.scan()

main()
