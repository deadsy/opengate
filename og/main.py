# open gate

import hd44780


def main():
    lcd = hd44780.LCD(4, (1, 2, 3, 4, 5, 6), 2, 16)
    lcd.test()
    lcd.puts(0, 0, "OpenGate")

main()
